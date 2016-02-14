#pragma once

#include <stdint.h>
#include <stdbool.h>

typedef uint16_t    elf32_half_t;
typedef uint32_t    elf32_off_t;
typedef uint32_t    elf32_addr_t;
typedef uint32_t    elf32_word_t;
typedef int32_t     elf32_sword_t;

// TODO move to elf32.c
#define ELF_NIDENT  16
typedef struct {
    uint8_t         e_ident[ELF_NIDENT];
    elf32_half_t    e_type;
    elf32_half_t    e_machine;
    elf32_word_t    e_version;
    elf32_addr_t    e_entry;
    elf32_off_t     e_phoff;
    elf32_off_t     e_shoff;
    elf32_word_t    e_flags;
    elf32_half_t    e_ehsize;
    elf32_half_t    e_phentsize;
    elf32_half_t    e_phnum;
    elf32_half_t    e_shentsize;
    elf32_half_t    e_shnum;
    elf32_half_t    e_shstrndx;
} __attribute__((packed)) elf32_ehdr_t;


bool elf32_is_supported(const elf32_ehdr_t *);
void *elf32_load_file(void *);
const char *elf32_get_strerror(void);
