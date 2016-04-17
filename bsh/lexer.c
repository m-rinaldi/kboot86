#include <lexer.h>
#include <stddef.h>

enum {
    BASE_OCT,
    BASE_DEC,
    BASE_HEX
};

static struct {
    const char      *begin;
    const char      *forward;
    bool            eoi_reached;   

    token_t         token;
    bool            token_available;
} _;

static inline
char _cur_char(void)
{
    return *_.forward;
}

static inline
void _move_forward(void)
{
    _.forward++;
}

static inline 
void _flush_buffer(void)
{
    _.begin = _.forward;
}

static inline
bool _char_is_skippable(const char c)
{
    return ' ' == c;
}

static inline
bool _char_is_alpha(const char c)
{
    return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || '_' == c;
}

static inline
bool _char_is_digit(const char c)
{
    return c >= '0' && c <= '9'; 
}

static inline
bool _char_is_oct(const char c)
{
    return c >= '0' && c <= '7';
}

static inline
bool _char_is_hex(const char c)
{
    return  _char_is_digit(c)       || 
            (c >= 'a' && c <= 'f')  ||
            (c >= 'A' && c <= 'F');
}

static inline
bool _char_is_alphanum(const char c)
{
    return _char_is_alpha(c) || _char_is_digit(c);
}

int lexer_init(const char *line)
{
    if (!line)
        return 1;

    _.begin = _.forward = line;
    
    _.token_available = false;
    _.eoi_reached = false;

    return 0;
}

static inline
void _scan(void)
{
    while (_char_is_skippable(_cur_char()))
        _move_forward();
}

static inline
void _set_token(token_t token)
{
    _.token_available = true;
    _.token = token;
}

static token_t _process_eoi(void)
{
    token_t token;

    token_set_eoi(&token);

    return token;
}

static token_t _process_identifier(void)
{
    token_t token;

    // current character already belongs to the identifier
    do {
        _move_forward();
    } while (_char_is_alphanum(_cur_char()));

    token_init(&token);
    token_set_identifier(&token, _.begin, _.forward-1);

    return token;
}

static bool _is_valid16(const char c)
{
    return  (c >= '0' && c <= '9')  ||
            (c >= 'a' && c <= 'f')  ||
            (c >= 'A' && c <= 'F');
}

static token_t _process_luinteger(void)
{
    token_t token;
    unsigned int base;

    if ('0' == _cur_char()) {
        switch (* (_.forward + 1)) {
            case 'x':
            case 'X':
                base = BASE_HEX;
                _.forward += 2;
                break;
            default:
                base = BASE_OCT;
        }
    } else
        base = BASE_DEC;

    // at least one additional digit is required for hexadecimal
    if (BASE_HEX == base && !_char_is_hex(_cur_char())) {

        token_set_error(&token, ERR_WRONG_NUM);
        return token;
    }

    do {
        _move_forward();    // checked just above
    } while (_is_valid16(_cur_char()));

    switch (base) {
        case BASE_OCT:
            token_set_luinteger8(&token, _.begin, _.forward-1);
            break;

        case BASE_DEC:
            token_set_luinteger10(&token, _.begin, _.forward-1);
            break;

        case BASE_HEX:
            token_set_luinteger16(&token, _.begin+2, _.forward-1);
            break;
        
        default:
            goto error;
    }

    return token;

error:
    token_set_error(&token, ERR_GENERIC);
    return token;
}

static token_t _process_variable(void)
{
    token_t token;

    // first character is the sigil
    do {
        _move_forward();
    } while (_char_is_alphanum(_cur_char()));

    // keep the sigil in the lexeme until the variable is added to the table
    // it indicates the type of the variable
    token_set_variable(&token, _.begin, _.forward-1);

    return token;
}

static token_t _process_string(void)
{
    token_t token;

    do {
        _move_forward();
    } while ('!' != _cur_char());

    // trailing <!>
    _move_forward();

    // remove both the leading and trailing <!>
    token_set_lstring(&token, _.begin+1, _.forward-2);

    return token;
}

static token_t _process_unknown(void)
{
    token_t token;

    do {
        _move_forward();
    } while (' ' != _cur_char());

    token_set_unknown(&token);

    return token;
}

void lexer_process(void)
{
    token_t token;
    token_t (*process_token)(void) = NULL;

    if (_.eoi_reached)
        return;

    _scan();
    _flush_buffer();

    switch (_cur_char()) {
        case '\0':
            process_token = _process_eoi;
            _.eoi_reached = true;
            break;

        case '$':
        case '%':
        case '?':
            process_token = _process_variable;
            break;

        case '!':
            process_token = _process_string;
            break;

        default:
            // identifer
            if (_char_is_alpha(_cur_char())) {
                process_token = _process_identifier;
                break;
            }

            // literal unsigned integer without prefix sign
            if (_char_is_digit(_cur_char())) {
                process_token = _process_luinteger;
                break;
            }

            // unknown
            process_token = _process_unknown;
    }

    token = process_token();
    _set_token(token);
}


bool lexer_has_token(void)
{
    return _.token_available;
}

token_t lexer_get_token(void)
{
    _.token_available = false;
    return _.token;
} 
