#pragma once

#include <stdbool.h>
#include <stdint.h>

typedef uint16_t    elf32_half_t;
typedef uint32_t    elf32_off_t;
typedef uint32_t    elf32_addr_t;
typedef uint32_t    elf32_word_t;
typedef int32_t     elf32_sword_t;

#define ELF_NIDENT  16
typedef struct {
    uint8_t         e_ident[ELF_NIDENT];
    elf32_half_t    e_type;
    elf32_half_t    e_machine;
    elf32_word_t    e_version;
    elf32_addr_t    e_entry;
    elf32_off_t     e_phoff;        // program header table offset
    elf32_off_t     e_shoff;        // section header table offset
    elf32_word_t    e_flags;
    elf32_half_t    e_ehsize;       // elf header size
    elf32_half_t    e_phentsize;    // size of a program header table entry
    elf32_half_t    e_phnum;        // number of entries in the program header
    elf32_half_t    e_shentsize;    // size of a section header table entry
    elf32_half_t    e_shnum;        // number of entries in the section header
    // index of the section header table entry that contains the section names
    elf32_half_t    e_shstrndx;
} __attribute__((packed)) elf32_ehdr_t;

bool elf32_is_supported(const elf32_ehdr_t *hdr);
uintptr_t elf32_map(const void *);
const char *elf32_strerror(void);

// TODO change "cost void *" to "elf32_ehdr_t *"
void elf32_display(const void *);
void elf32_display_pentries(const void *);
void elf32_display_symtable(const void *);
