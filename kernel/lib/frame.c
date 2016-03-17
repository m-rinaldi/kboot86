#include <frame.h>

#include <stdbool.h>

// TODO has to be defined elsewhere
#define PAGE_SIZE       (1 << 12)           // 4kb

#define PADDR_MIN       (2 << 20)           // 2mb 
#define PADDR_MAX       ((14 << 20) - 1)    // 14mb - 1
#define PMEM_AVAIL      (PADDR_MAX - PADDR_MIN + 1)
#define FRAMES_AVAIL    (PMEM_AVAIL / PAGE_SIZE)


#define LEN             (FRAMES_AVAIL / 32)
// bitmap, each bit corresponds to a frame
//      0 => free
//      1 => used
static uint32_t _[LEN];

static inline
void _set_bit(uint32_t *bitmap, unsigned int bit_num)
{
    *bitmap |= 1 << bit_num;
}

static inline
void _clear_bit(uint32_t *bitmap, unsigned int bit_num)
{
    *bitmap &= ~(1 << bit_num);
}

static inline
bool _is_bit_set(uint32_t bitmap, unsigned int bit_num)
{
    return !!(bitmap & (1 << bit_num));
}

// get the index of least significant bit that is cleared
static inline
unsigned int _get_clear_bit_num(uint32_t bitmap)
{
    unsigned int i;
    
    for (i = 0; i < 8 * sizeof(bitmap); i++)
        if (!_is_bit_set(bitmap, i))
            return i;

    return ~0;
}


static bool _has_free_frame(uint32_t bitmap)
{
    return bitmap != ~((uint32_t) 0);
}

static inline
uint32_t _idx2paddr(unsigned int i, unsigned int j)
{
    return PADDR_MIN + PAGE_SIZE * (32 * i + j);
}

static inline
int _paddr2idx(uint32_t paddr, unsigned int *i, unsigned int *j)
{
    uint32_t page_num = (paddr - PADDR_MIN) / PAGE_SIZE;
    
    if (i) *i = page_num / 32;
    if (j) *j = page_num % 32;

    return 0;
}

// allocation time complexity is O(n)
uint32_t frame_alloc(void)
{
    unsigned int i, j;

    for (i = 0; i < LEN; i++) {
        if (!_has_free_frame(_[i]))
            continue;

        j = _get_clear_bit_num(_[i]);

        // mark frame as used
        _set_bit(&_[i], j);

        return _idx2paddr(i, j);
    }

    // no frames available        
    return 0;
}

// deallocation time complexity is O(1)
int frame_dealloc(uint32_t paddr)
{
    unsigned int i, j;

    // TODO use the function _is_page_aligned_addr()
    // page-aligned address?
    if (paddr & 0x3ff)
        return 1;

    if (_paddr2idx(paddr, &i, &j))
        return 1;

    if (i >= LEN)
        return 1;

    if (!_is_bit_set(_[i], j))
        return 1; 

    // mark frame as unused
    _clear_bit(_ + i, j);

    return 0;
}
