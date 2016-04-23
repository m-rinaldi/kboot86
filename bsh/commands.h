// the semantics of the command are defined here

static inline
int _display_cmd_name(const cmd_table_entry_t *cte)
{
    kprintf("%s\n", cte->name);
    return 0;
}

#define CMD_lscmd_arity 0
#define CMD_lscmd_desc  "display the list of all the commands"
#define CMD_lscmd_usage "lscmd"
static
int _cmd_lscmd(void)
{
    _foreach_cmd_entry(_display_cmd_name);
    return 0;
}

#define CMD_help_arity  1
#define CMD_help_desc   "display the help for the given command"
#define CMD_help_usage  "help set"
static
int _cmd_help(const token_t *cmd_name)
{
    cmd_table_entry_t *cte;
    const char *_cmd_name;

    if (!token_is_identifier(cmd_name)) {
        kprintf("wrong parameters, not a command name\n"); 
        return 1;
    }

    _cmd_name = cmd_name->_.str;

    if (!(cte = _get_cmd_entry(_cmd_name))) {
        kprintf("no command with the name \"%s\"\n", _cmd_name);
        return 1;
    }

    kprintf("%s", cte->name);
    kprintf(" - %s\n", cte->description);
    kprintf("usage:\n\t%s\n", cte->example);
    
    
    return 0; 
}

#define CMD_halt_arity  0
#define CMD_halt_desc   "shutdowns the computer"
#define CMD_halt_usage  "shutdown"
static
int _cmd_halt(void)
{
    asm volatile (
        "# _cmd_halt\n\t"
        "hlt\n\t"
    );
    return 0;
}


#define CMD_set_arity   2
#define CMD_set_desc    "sets a variable to the given value"
#define CMD_set_usage   "set $var 113\n\tset %var !hello world!\n\tset ?var"
static
int _cmd_set(const token_t *var_name, const token_t *value)
{
    if (var_set_entry(var_name, value))
        return 1;
    return 0;
}

static inline
const char *_type_str(unsigned int type)
{
    switch (type) {
        case TYPE_STR:
            return "str ";
        
        case TYPE_INT:
            return "int ";

        case TYPE_BOOL:
            return "bool";

        default:
            return "?";
    }
}

static
int _display_var(var_entry_t *ve)
{
    kprintf("%s(%s)\t", ve->name, _type_str(ve->type));
    switch (ve->type) {
        case TYPE_STR:
            kprintf("\"%s\"", ve->_.str_val);
            break;
        
        case TYPE_INT:
            kprintf("0x%x", ve->_.int_val);
            break;

        case TYPE_BOOL:
            kprintf("%s", ve->_.bool_val ? "true" : "false");
            break;
    
        default:
            kprintf("???");
    }
    kprintf("\n");

    return 0;
}

#define CMD_show_arity  1
#define CMD_show_desc   "shows the value of a variable"
#define CMD_show_usage  "show $var"
static
int _cmd_show(const token_t *var_name)
{
    var_entry_t *ve;

    if (!(ve = var_get_entry(var_name))) {
        kprintf("no variable with the name \"%s\"\n", var_name->_.str);
        return 1;
    }

    // TODO display value

    return 0;
}


#define CMD_vars_arity  0
#define CMD_vars_desc   "displays all defined variables and their values"
#define CMD_vars_usage  "vars"
static
int _cmd_vars(void)
{
    return var_foreach(_display_var);
}