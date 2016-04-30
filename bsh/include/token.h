#pragma once

#include <stdint.h>
#include <stdbool.h>

#define SIGIL_STR_VAR       '$'
#define _SIGIL_STR_VAR      "$"

#define SIGIL_INT_VAR       '%'
#define _SIGIL_INT_VAR      "%"

#define SIGIL_BOOL_VAR      '.'
#define _SIGIL_BOOL_VAR     "."

#define DELIMITER_STR       '!'
#define _DELIMITER_STR      "!"

typedef enum {
    VOID = 0,       // on inizialization
    ERROR,
    UNKNOWN,
    IDENTIFIER,
    VARIABLE,
    LSTRING,        // literal string
    LUINTEGER,      // literal unsigned integer
    EOI,            // end of input
} token_type_t;

typedef enum {
    ERR_GENERIC = 0,
    ERR_NUM_STR_TOO_LONG,
    ERR_STR_TOO_LONG,
    ERR_NUM_STR_TOO_SHORT,
    ERR_WRONG_NUM,
} token_error_t;

#define TOKEN_STR_LEN_MAX   15    

typedef struct {
    token_type_t    type;
    union {
        char            str[TOKEN_STR_LEN_MAX+1];
        uint32_t        integer;
        bool            boolean;
        token_error_t   err_code;
    } _;
} token_t;

void token_init(token_t *);
void token_set_identifier(token_t *, const char *, const char *);
void token_set_variable(token_t *, const char *, const char *);
void token_set_error(token_t *, token_error_t);
void token_set_unknown(token_t *);
void token_set_eoi(token_t *);

void token_set_luinteger8(token_t *, const char *, const char *);
void token_set_luinteger10(token_t *, const char *, const char *);
void token_set_luinteger16(token_t *, const char *, const char *);

void token_set_lstring(token_t *, const char *, const char *);

bool token_is_identifier(const token_t *);
bool token_is_luinteger(const token_t *);
bool token_is_lstring(const token_t *);
bool token_is_unknown(const token_t *);
bool token_is_error(const token_t *);
bool token_is_variable(const token_t *);
bool token_is_eoi(const token_t *);

//XXX
void token_display(const token_t *token);

