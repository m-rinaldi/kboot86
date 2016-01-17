#include <shell.h>

#include <console.h>
#include <string.h>
#include <intr.h>

const char *prompt = "bsh-> ";

#define BUF_SIZE    512
// echo shell
void shell_do(void)
{
    char str[BUF_SIZE];
    size_t len;

    console_puts("welcome to the boot shell\n");
    while (1) {
        console_puts(prompt);
        len = BUF_SIZE-1;

        if (console_get_line(str, &len))
            continue;
        str[len] = '\0';

        console_puts(str);
        if (strlen(str))
            console_puts("\n");
    }
}
