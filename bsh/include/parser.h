#pragma once

#include <token.h>
#include <stddef.h>

#define SYNTAX_VEC_LEN  4


// TODO rename syntax_data_t to parser_data_t
typedef struct {
    token_t syntax_vec[SYNTAX_VEC_LEN];
} syntax_data_t;

syntax_data_t *parser_do(const char *);
const char *parser_strerror(void);
size_t parser_data_len(const syntax_data_t *);
