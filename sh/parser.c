#include <parser.h>

#include <token.h>
#include <lexer.h>

#include <stddef.h>

static syntax_data_t _;


typedef enum {
    ERR_TOO_MANY_TOKENS = 0,
    ERR_ERROR_TOKEN,
    ERR_UNKNOWN_TOKEN,
    ERR_INTERNAL
} err_code_t;


static err_code_t _err_code;
static const char *_err_str[] = 
    {
        "the given line contains too many elements",
        "error token",
        "unknown token",
        "internal error"
    };

static
void _reset(void)
{
    for (int i = 0; i < SYNTAX_VEC_LEN; i++)
        token_init(&_.syntax_vec[i]);
}

syntax_data_t *parser_do(const char *str)
{
    token_t token;
    int vec_idx;

    if (!str) {
        _err_code = ERR_INTERNAL;
        return NULL;
    }

    if (lexer_init(str)) {
        _err_code = ERR_INTERNAL;
        return NULL;
    }

    // all tokens to void
    _reset();
    
    for (lexer_init(str), vec_idx = 0; vec_idx < SYNTAX_VEC_LEN; vec_idx++)
    {
        lexer_process();

        if (!lexer_has_token()) {
            _err_code = ERR_INTERNAL;
            return NULL;    // EOI not received yet
        }

        token = lexer_get_token();

        if (token_is_error(&token)) {
            _err_code = ERR_ERROR_TOKEN;
            return NULL;
        }

        if (token_is_unknown(&token)) {
            _err_code = ERR_UNKNOWN_TOKEN;
            return NULL;
        }

        // add token to the syntax vector
        _.syntax_vec[vec_idx] = token;

        if (token_is_eoi(&token))
            break;
    }

    // the last token added should be EOI
    if (!token_is_eoi(&token)) {
        _err_code = ERR_TOO_MANY_TOKENS;
        return NULL;
    }
    
    return &_;
}

const char *parser_strerror(void)
{
    return _err_str[_err_code];
}


size_t parser_data_len(const syntax_data_t *sx)
{
    for (size_t i = 0; i < SYNTAX_VEC_LEN; i++)
        if (token_is_eoi(&sx->syntax_vec[i]))
            return i;

    return -1;
}
