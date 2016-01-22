// make an incremental (hdd) image, i.e.: an image with the following content:
//      1st 32-bit word:    0
//      2nd 32-bit word:    1
//      3rd 32-bit word:    2
//      ...
//      nth 32-bit word:  n-1
//
// useful for testing whether a mass-storage device driver is working properly

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#define LEN (512/sizeof(uint32_t))
typedef uint32_t sector_t[LEN];

static struct {
    FILE *file;
    unsigned int cylinders;
    unsigned int headers;
    unsigned int spt;

    uint32_t    counter;
    sector_t     sector;
} _;


static void _show_usage(const char *prog_name)
{
    fprintf(stderr, "%s: cylinders headers sectors_per_track image_name\n",
            prog_name);
}

static FILE *_open_file(const char *filepath)
{
    FILE *f;

    f = fopen(filepath, "w+");
    return f;
}

static
int _init(const char *filepath,
          unsigned int c, unsigned int h, unsigned int s)
{
    if (!filepath || !c || !h || !s)
        return 0;

    if (!(_.file = _open_file(filepath)))
        return -1;

    _.cylinders = c;
    _.headers = h;
    _.spt = s;
    
    _.counter = 0;
    
    return 0; 
}

static void _update_sector(void)
{
    for (unsigned int i = 0; i < sizeof(_.sector) / sizeof(uint32_t); i++)
        _.sector[i] = _.counter++;
}

static int _write_sector(void)
{
    _update_sector();
    fwrite(_.sector, sizeof(sector_t), 1, _.file);    
    return 0;
}


static int _expand_image(void)
{
    for (unsigned int c = 0; c < _.cylinders; c++)
        for (unsigned int h = 0; h < _.headers; h++)
            for (unsigned int s = 0; s < _.spt; s++)
                _write_sector(); 
    return 0;
}

// program cylinders headers sectors_per_track image_name
int main(int argc, char *argv[])
{
    if (argc != 5) {
        _show_usage(argv[0]);
        exit(1);
    }

    if (_init(argv[4], atoi(argv[1]), atoi(argv[2]), atoi(argv[3])))
        return 1;

    if (_expand_image())
        return 1;

    printf("cylinders: %6d\n", _.cylinders);
    printf("headers:   %6d\n", _.headers);
    printf("spt:       %6d\n", _.spt);
    
    return 0;
}
