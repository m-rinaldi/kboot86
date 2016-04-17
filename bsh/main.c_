#include <stdio.h>
#include <shell.h>
#include <lexer.h>
#include <token.h>

#if 0
static char buf[1024];
static token_t token[8];
#endif

int main(void)
{
#if 0
    int i;
    printf("enter text: "); fflush(stdout);
    gets(buf);
    fflush(stdin);
    printf("   text entered: %s\n", buf);

    lexer_init(buf);

    for(i = 0, lexer_process();
        lexer_has_token() && i < 8;
        i++, lexer_process())
    {
        printf("\thas token\n");
        token[i] = lexer_get_token();
        token_display(&token[i]);
    }
#endif

    shell_do();

    return 0;
}
