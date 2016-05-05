#include <shell.h>

#include <parser.h>
#include <token.h>
#include <var_table.h>

#include <string.h>
#include <console.h>
#include <kstdio.h>

#define BUF_SIZE    512     
static char _buf[BUF_SIZE];

typedef void* cmd_func_t;
typedef const token_t* cmd_arg_t;

typedef int (*cmd_func0_t)(void);
typedef int (*cmd_func1_t)(cmd_arg_t);
typedef int (*cmd_func2_t)(cmd_arg_t, cmd_arg_t);

typedef struct {
    const char *    name;
    unsigned int    arity;
    cmd_func_t      cmd_func;

    const char *    description;
    const char *    example;
} cmd_table_entry_t;

static cmd_table_entry_t *_get_cmd_entry(const char *);
static void _foreach_cmd_entry(int (*func)(const cmd_table_entry_t *));
#include "commands.h"

#define ADD_CMD(name)   { \
                            #name, CMD_ ## name ## _arity,  \
                            (cmd_func_t) _cmd_ ## name,     \
                            CMD_ ## name ## _desc,          \
                            CMD_ ## name ## _usage,         \
                        },

static cmd_table_entry_t _cmd_table[] = {
    ADD_CMD(lscmd)
    ADD_CMD(help)
    ADD_CMD(set)
    ADD_CMD(show)
    ADD_CMD(vars)
};
static const size_t _cmd_table_len = sizeof(_cmd_table) / sizeof(_cmd_table[0]);


static cmd_table_entry_t *_get_cmd_entry(const char *name)
{
    for (unsigned int i = 0; i < _cmd_table_len; i++)
        if (!strcmp(name, _cmd_table[i].name))
            return _cmd_table + i;

    return NULL;
}

// iterate over the command table
static void _foreach_cmd_entry(int (*func)(const cmd_table_entry_t *))
{
    for (size_t i = 0; i < _cmd_table_len; i++)
        if (func(_cmd_table + i))
            break;
}

// perform the call based on the arity
static
int _call_cmd(cmd_func_t cmd_func, unsigned int arity, const token_t *tokens)
{
    // TODO
   
    switch (arity) {
        case 0:
            return ((cmd_func0_t) cmd_func)();

        case 1:
            return ((cmd_func1_t)cmd_func)(&tokens[1]);

        case 2:
            return ((cmd_func2_t)cmd_func)(&tokens[1], &tokens[2]);

        default:
            return 1;
    } 

    return 0;
}

// TODO each command should be implemented in a function in that it will be
//      taken care of the type of the received arguments

// TODO before calling that function, it will be tested only the arity of
//      the command

static const char *_prompt = "shell-> ";

void shell_do(void)
{
    syntax_data_t *sx;
    cmd_table_entry_t *cte;
    size_t len;

    kprintf("welcome to the boot shell\n");

    while (1) {
        _buf[0] = '\0';
        len = BUF_SIZE - 1;

        kprintf("%s", _prompt);

        if (console_get_line(_buf, &len))
            continue;
        _buf[len] = '\0';

        if (!(sx = parser_do(_buf))) {
            kprintf("syntax error: ");
            kprintf("%s\n", parser_strerror());
            continue;
        }

        // blank line
        if (token_is_eoi(&sx->syntax_vec[0]))
            continue;

        if (!token_is_identifier(&sx->syntax_vec[0])) {
            kprintf("semantic error: ");
            kprintf("first word must be an identifier\n");
            continue;
        }
        
        if (!(cte = _get_cmd_entry(sx->syntax_vec[0]._.str))) {
            kprintf("no command \"%s\" found\n", sx->syntax_vec[0]._.str);   
            continue;
        }

        if (cte->arity != (parser_data_len(sx) - 1)) {
            kprintf("command \"%s\" takes %d parameter(s)\n",
                    cte->name, cte->arity);
            // display the help for this command
            _cmd_help(&sx->syntax_vec[0]);            
            continue;
        }

        if (_call_cmd(cte->cmd_func, cte->arity, sx->syntax_vec))
            kprintf("command \"%s\" error\n", sx->syntax_vec[0]._.str);

    }
}

