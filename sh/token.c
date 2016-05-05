#include <token.h>

#include <string.h>
#include <stdbool.h>
#include <kstdio.h>

static inline
size_t _str_len(const char *begin, const char *end)
{
    return end - begin + 1;
}

bool token_is_void(const token_t *token)
{
    return VOID == token->type;
}

bool token_is_unknown(const token_t *token)
{
    return UNKNOWN == token->type;
}

bool token_is_identifier(const token_t *token)
{
    return IDENTIFIER == token->type;
}

bool token_is_variable(const token_t *token)
{
    return VARIABLE == token->type;
}

bool token_is_luinteger(const token_t *token)
{
    return LUINTEGER == token->type;
}

bool token_is_lstring(const token_t *token)
{
    return LSTRING == token->type;
}

bool token_is_error(const token_t *token)
{
    return ERROR == token->type;
}

bool token_is_eoi(const token_t *token)
{
    return EOI == token->type;
}

void token_init(token_t *token)
{
    token->type = VOID;
}

void token_set_eoi(token_t *token)
{
    token->type = EOI;
}

void token_set_error(token_t *token, token_error_t error)
{
    token->type = ERROR;
    token->_.err_code = error;
}

void token_set_unknown(token_t *token)
{
    token->type = UNKNOWN;
}

static
void _set_strlex(token_t *token, token_type_t type,
                   const char *begin, const char *end)
{
    if (_str_len(begin, end) > TOKEN_STR_LEN_MAX) {
        token_set_error(token, ERR_STR_TOO_LONG);
        return;
    }

    token->type = type;
    strncpy(token->_.str, begin, _str_len(begin, end));
    token->_.str[_str_len(begin, end)] = '\0';
}

void token_set_identifier(token_t *token, const char *begin, const char *end)
{
    _set_strlex(token, IDENTIFIER, begin, end);
}

void token_set_variable(token_t *token, const char *begin, const char *end)
{
    _set_strlex(token, VARIABLE, begin, end);
}

void token_set_lstring(token_t *token, const char *begin, const char *end)
{
    _set_strlex(token, LSTRING, begin, end);
}

static
unsigned int _8char2int(const char c)
{
    if (c >= '0' && c <= '7')
        return c - '0';

    // error
    return -1;
}

static
unsigned int _10char2int(const char c)
{
    if (c >= '0' && c <= '9')
        return c - '0';

    // error
    return -1;
}

static
unsigned int _16char2int(const char c)
{
    if (c >= '0' && c <= '9')
        return c - '0';

    if (c >= 'a' && c <= 'f')
        return 10 + c - 'a';

    if (c >= 'A' && c <= 'F')
        return 10 + c - 'A';

    // error
    return -1;
}

static 
void _set_luinteger(token_t *token, unsigned int base,
                    unsigned int (*converter)(const char),
                    const char *begin, const char *end)
{
    unsigned int number, digit, factor;

    number = 0;
    factor = 1;

    do {
        char c = *end;

        digit = converter(c);
        if (-1 == (int) digit)
            goto error;

        number += digit * factor;
        factor *= base;
    } while (end-- > begin);

    token->type = LUINTEGER;
    token->_.integer = number;

    return;

error:
    token->type = ERROR;
    token->_.err_code = ERR_WRONG_NUM;
}

void token_set_luinteger8(token_t *token, const char *begin, const char *end)
{
    if (_str_len(begin, end) > 10 + 1) {
        token_set_error(token, ERR_NUM_STR_TOO_LONG);
        return;
    }
    _set_luinteger(token, 8, _8char2int, begin, end);     
}

void token_set_luinteger10(token_t *token, const char *begin, const char *end)
{
    if (_str_len(begin, end) > 9) {
        token_set_error(token, ERR_NUM_STR_TOO_LONG);
        return;
    }
    _set_luinteger(token, 10, _10char2int, begin, end); 
}

void token_set_luinteger16(token_t *token, const char *begin, const char *end)
{
    if (_str_len(begin, end) > 8) {
        token_set_error(token, ERR_NUM_STR_TOO_LONG);
        return;
    }
    _set_luinteger(token, 16, _16char2int, begin, end); 
}

void token_display(const token_t *token)
{
    kprintf("TOKEN\n");
    if (token_is_void(token)) {
        kprintf("\tvoid\n");
    } else if (token_is_identifier(token)) {
        kprintf("\tidentifier\n");
        kprintf("\t<%s>\n", token->_.str);
    } else if (token_is_variable(token)) {
        kprintf("\tvariable\n");
        kprintf("\t<%s>\n", token->_.str);
    } else if (token_is_lstring(token)) {
        kprintf("\tlstring\n");
        kprintf("\t<%s>\n", token->_.str);
    } else if (token_is_eoi(token)) {
        kprintf("\tEOI\n");
    } else if (token_is_luinteger(token)) {
        kprintf("\tluinteger\n");
        kprintf("\t<0x%x>\n", token->_.integer);  
    } else if (token_is_error(token)) {
        kprintf("\terror\n");
    } else if (token_is_unknown(token)) {
        kprintf("\t%s...\n", token->_.str);
    }
}
