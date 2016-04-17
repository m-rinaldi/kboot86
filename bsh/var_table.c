#include <var_table.h>

#include <stddef.h>
#include <stdbool.h>
#include <string.h>
#include <token.h>

#define TABLE_LEN   8

static var_entry_t _[TABLE_LEN];

static void _init_entry(var_entry_t *vte)
{
    vte->used = false;
}

void var_table_init(void)
{
    for (int i = 0; i < TABLE_LEN; i++)
        _init_entry(&_[i]);
}

static
var_entry_t *_get_entry(const char *name, int type)
{
    for (int i = 0; i < TABLE_LEN; i++) {
        if (!_[i].used)
            continue;

        if (type != _[i].type)
            continue;

        if (!strcmp(_[i].name, name))
            return _ + i;
    }

    return NULL;
}

static
var_entry_t *_get_free_entry(void)
{
    for (int i = 0; i < TABLE_LEN; i++)
        if (!_[i].used) {
            _[i].used = true;
            return _ + i; 
        }

    // no free entry
    return NULL;
}

// determine the type of the variable token based on its sigil
static inline
int _get_variable_token_type(const token_t *var_name)
{
    switch (var_name->_.str[0]) {
        case '$':
            return TYPE_STR;            

        case '%':
            return TYPE_INT;            

        case '?':
            return  TYPE_BOOL;

        default:
            return -1;
    }
}

int var_set_entry(const token_t *var_name, const token_t *val)
{
    int type;
    var_entry_t *vte;

    if (!token_is_variable(var_name))
        return 1;


    if (-1 == (type = _get_variable_token_type(var_name)))
        return 1;

    // check the type of the val parameter
    switch (type) {
        case TYPE_STR:
            if (!token_is_lstring(val))
                return 1;
            break;
    
        case TYPE_INT:
        case TYPE_BOOL:
            if (!token_is_luinteger(val))
                return 1;
            break;

    }
   
    // get the variable entry if it exists already in the table
    if (!(vte = _get_entry(var_name->_.str+1, type))) {
        if (!(vte = _get_free_entry())) {
            // no free entries
            return 1;
        } else {
            // set the name of the new allocated variable entry
            strncpy(vte->name, var_name->_.str+1, VAR_NAME_STR_LEN_MAX);
            vte->name[VAR_NAME_STR_LEN_MAX] = '\0';
        }
    }


    // set the value and the type of the variable entry regardless of
    // the old type and value
    vte->type = type;

    switch (vte->type) {
        case TYPE_STR:
            strncpy(vte->_.str_val, val->_.str, VAR_VAL_STR_LEN_MAX);
            vte->_.str_val[VAR_VAL_STR_LEN_MAX] = '\0';
            break;

        case TYPE_INT:
            vte->_.int_val = val->_.integer;
            break;

        case TYPE_BOOL:
            vte->_.bool_val = val->_.boolean;
            break;
    }


    return 0;
}

var_entry_t *var_get_entry(const token_t *var_name)
{
    int type;

    if (!token_is_variable(var_name))
        return NULL;

    if (-1 == (type = _get_variable_token_type(var_name)))
        return NULL;

    return _get_entry(var_name->_.str+1, type);
}

int var_foreach(int (*do_func)(var_entry_t * ve))
{
    size_t i;

    for (i = 0; i < TABLE_LEN; i++) {
        if (!_[i].used)
            continue;
        if (do_func(_ + i))
            return 1;
    }

    return 0;   
}
