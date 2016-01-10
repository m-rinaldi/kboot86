#include <shell.h>
#include <console.h>

#define BUF_SIZE    32
// echo shell
void shell_do(void)
{
    char str[BUF_SIZE];
    size_t len;

    console_puts("\n-> ");
    
    while (1) {
        len = BUF_SIZE-1;
        console_get_line(str, &len);
        str[len] = '\0';

        console_puts(str);
        console_puts("\n");
        console_puts("-> ");
    }
}
