#include <elf32.h>

#include <stddef.h>

enum elf_ident {
    EI_MAG0 = 0,    // 0x7f
    EI_MAG1,        // 'E'
    EI_MAG2,        // 'L'
    EI_MAG3,        // 'F'
    EI_CLASS,       // arch: 1 => 32 bit, 2 => 64 bit
    EI_DATA,        // endianness: 1 => little, 2 => big
    EI_VERSION,     // ELF version
    EI_OSABI,       // OS specific
    EI_ABIVERSION,  // OS specific
    EI_PAD          // padding
};

#define ELF_MAG0            0x7f
#define ELF_MAG1            'E'
#define ELF_MAG2            'L'
#define ELF_MAG3            'F'
#define ELF_DATA_LITTLE     1   // little endian
#define ELF_CLASS_32        1   // 32-bit arch
#define ELF_TYPE_REL        1   // relocatable
#define ELF_TYPE_EXEC       2   // executable
#define ELF_MACHINE_386     3   // x86 machine type
#define ELF_VERSION_CURRENT 1   // ELF current version

// ELF32 section header
typedef struct {
    elf32_word_t    sh_name;    // relative to e_shstrndx in elf32_ehdr_t
    elf32_word_t    sh_type;
    elf32_word_t    sh_flags; 
    elf32_addr_t    sh_addr;
    elf32_off_t     sh_offset;
    elf32_word_t    sh_size;
    elf32_word_t    sh_link;
    elf32_word_t    sh_info;
    elf32_word_t    sh_addralign;
    elf32_word_t    sh_entsize;  
} __attribute__((packed)) elf32_shdr_t;

enum sh_types {
    SHT_NULL    = 0,    // null section
    SHT_PROGBITS,       // program information
    SHT_SYMTAB,         // symbol table
    SHT_STRTAB,         // string table
    SHT_RELA,           // relocation (with addend)
    SHT_NOBITS  = 8,    // not present in file
    SHT_REL             // relocation (no addend)
};

enum sh_attr {
    SHF_WRITE   = 1,    // write section
    SHF_ALLOC   = 2     // exists in memory
};

// ELF32 program header
typedef struct {
    elf32_word_t    p_type;
    elf32_off_t     p_offset;
    elf32_addr_t    p_vaddr;
    elf32_addr_t    p_paddr;
    elf32_word_t    p_filesz;
    elf32_word_t    p_memsz;
    elf32_word_t    p_flags;
    elf32_word_t    p_align;    
} __attribute__((packed)) elf32_phdr_t;


static const char *_str_error = "unknown error";

static bool _is_valid(const elf32_ehdr_t *hdr)
{
    if (hdr->e_ident[EI_MAG0] != ELF_MAG0)
        return false;

    if (hdr->e_ident[EI_MAG1] != ELF_MAG1)
        return false;

    if (hdr->e_ident[EI_MAG2] != ELF_MAG2)
        return false;

    if (hdr->e_ident[EI_MAG3] != ELF_MAG3)
        return false;

    return true;
}

bool elf32_is_supported(const elf32_ehdr_t *hdr)
{
    if (!hdr)
        return false;

    if (!_is_valid(hdr)) {
        _str_error = "invalid ELF magin number";
        return false;
    }

    if (hdr->e_ident[EI_CLASS] != ELF_CLASS_32) {
        _str_error = "not 32-bit";
        return false;
    }

    if (hdr->e_ident[EI_DATA] != ELF_DATA_LITTLE) {
        _str_error = "not little-endian";
        return false;
    }

    if (hdr->e_machine != ELF_MACHINE_386) {
        _str_error = "not 386";
        return false;
    }

    if (hdr->e_type != ELF_TYPE_EXEC) {
        _str_error = "not executable";
        return false;
    }

    return true;
}

static inline void *_load_rel(elf32_ehdr_t *hdr)
{
    // TODO
    return NULL;
}

void *elf32_load_file(void *elf_image)
{
    elf32_ehdr_t *hdr;

    hdr = (elf32_ehdr_t *) elf_image;
    if (!elf32_is_supported(hdr))
        return NULL;

    if (hdr->e_type != ELF_TYPE_REL)
        return NULL;

    return _load_rel(hdr);
}

const char *elf32_strerror(void)
{
    return _str_error;
}

void elf32_display_phdr(void *elf_image)
{
    elf32_phdr_t *hdr = (elf32_phdr_t *) elf_image;        
}
