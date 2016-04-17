#pragma once

#include <stdbool.h>

#include <token.h>

#define VAR_NAME_STR_LEN_MAX    TOKEN_STR_LEN_MAX
#define VAR_VAL_STR_LEN_MAX     15

#define TYPE_STR    1
#define TYPE_INT    2
#define TYPE_BOOL   4

typedef struct {
    char            name[VAR_NAME_STR_LEN_MAX+1]; 
    bool            used; 

    int             type; 
    struct {
        unsigned int    int_val;
        char            str_val[VAR_VAL_STR_LEN_MAX+1];
        bool            bool_val;
    } _;
} var_entry_t;

static inline
bool var_entry_is_int(var_entry_t *vte)
{
    return TYPE_INT == vte->type;
}

static inline
bool var_entry_is_str(var_entry_t *vte)
{
    return TYPE_STR == vte->type;
}

