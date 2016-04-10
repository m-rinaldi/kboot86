#include <kstdio.h>

#include <stdarg.h>
#include <stddef.h>
#include <string.h>
#include <console.h>


#define BUF_SIZE	1024
static char _buf[BUF_SIZE];


int kprintf(const char *format, ...)
{
	va_list args;
	int i;

	va_start(args, format);
	i = kvsprintf(_buf, format, args);
	va_end(args);

    console_puts(_buf);

	return i;
}

int ksprintf(char *str, const char *format, ...)
{
    va_list args;
    int i;

    va_start(args, format);
    i = kvsprintf(str, format, args);
    va_end(args);

    return i;
}

/* we use this so that we can do without the ctype library */
#define is_digit(c) ((c) >= '0' && (c) <= '9')

static int skip_atoi(const char **s)
{
    int i=0;

    while (is_digit(**s))
        i = i*10 + *((*s)++) - '0';
    return i;
}

#define ZEROPAD 1       /* pad with zero */
#define SIGN    2       /* unsigned/signed long */
#define PLUS    4       /* show plus */
#define SPACE   8       /* space if plus */
#define LEFT    16      /* left justified */
#define SPECIAL 32      /* 0x */
#define SMALL   64      /* use 'abcdef' instead of 'ABCDEF' */

/* the following code was taken from Linux 0.01 */
static inline
int do_div(int *num, const int base)
{
    int remainder;

    /*
        DIVL (unsigned division)
        dividend:   EDX:EAX
        divisor:    instruction operand
        quotient:   EAX
        reminder:   EDX
    */
    asm volatile (
                    "# do_div()\n\t"
                    "divl %4\n\t"
                    :"=a" (*num), "=d" (remainder)
                    :"0"  (*num), "1"  (0), "r" (base)
                );

    return remainder;
}

static char * number(char * str, int num, int base, int size, int precision
    ,int type)
{
    char c,sign,tmp[36];
    const char *digits="0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ";
    int i;

    if (type & SMALL)
        digits="0123456789abcdefghijklmnopqrstuvwxyz";

    if (type & LEFT)
        type &= ~ZEROPAD;

    if (base < 2 || base > 36)
        return 0;

    c = (type & ZEROPAD) ? '0' : ' ' ;

    if (type & SIGN && num<0) {
        sign='-';
        num = -num;
    } else
        sign=(type & PLUS) ? '+' : ((type & SPACE) ? ' ' : 0);
    if (sign)
        size--;
    if (type & SPECIAL) {
        if (16 == base)
            size -= 2;
        else if (8 == base)
                size--;
    }
    i=0;
    if (!num)
        tmp[i++]='0';
    else while (num) 
        tmp[i++]=digits[do_div(&num,base)];
    if (i > precision)
        precision=i;
    size -= precision;
    if (!(type & (ZEROPAD+LEFT)))
        while(size-->0)
            *str++ = ' ';

    if (sign)
        *str++ = sign;

    if (type & SPECIAL) {
        if (8 == base)
            *str++ = '0';
        else if (16 == base) {
            *str++ = '0';
            *str++ = digits[33];
        }
    }
    if (!(type & LEFT))
        while(size-->0)
            *str++ = c;

    while(i < precision--)
        *str++ = '0';

    while(i-- > 0)
        *str++ = tmp[i];

    while(size-- > 0)
        *str++ = ' ';

    return str;
}

int kvsprintf(char *buf, const char *fmt, va_list args)
{
    int len;
    int i;
    char * str;
    char *s;
    int *ip;

    int flags;          /* flags to number() */

    int field_width;    /* width of output field */
    int precision;      /* min. # of digits for integers; max
                           number of chars for from string */

    for (str=buf ; *fmt ; ++fmt) {
        if (*fmt != '%') {
            *str++ = *fmt;
            continue;
        }
            
        /* process flags */
        flags = 0;
        repeat:
            ++fmt;  /* this also skips first '%' */
            switch (*fmt) {
                case '-': flags |= LEFT; goto repeat;
                case '+': flags |= PLUS; goto repeat;
                case ' ': flags |= SPACE; goto repeat;
                case '#': flags |= SPECIAL; goto repeat;
                case '0': flags |= ZEROPAD; goto repeat;
            }
        
        /* get field width */
        field_width = -1;
        if (is_digit(*fmt))
            field_width = skip_atoi(&fmt);
        else if (*fmt == '*') {
            /* it's the next argument */
            field_width = va_arg(args, int);
            if (field_width < 0) {
                field_width = -field_width;
                flags |= LEFT;
            }
        }

        /* get the precision */
        precision = -1;
        if (*fmt == '.') {
            ++fmt;  
            if (is_digit(*fmt))
                precision = skip_atoi(&fmt);
            else if (*fmt == '*') {
                /* it's the next argument */
                precision = va_arg(args, int);
            }
            if (precision < 0)
                precision = 0;
        }
        if (*fmt == 'h' || *fmt == 'l' || *fmt == 'L') {
            ++fmt;
        }

        switch (*fmt) {
        case 'c':
            if (!(flags & LEFT))
                while (--field_width > 0)
                    *str++ = ' ';
            *str++ = (unsigned char) va_arg(args, int);
            while (--field_width > 0)
                *str++ = ' ';
            break;

        case 's':
            s = va_arg(args, char *);
            len = strlen(s);
            if (precision < 0)
                precision = len;
            else if (len > precision)
                len = precision;

            if (!(flags & LEFT))
               while (len < field_width--)
                    *str++ = ' ';
            for (i = 0; i < len; ++i)
                *str++ = *s++;
            while (len < field_width--)
                *str++ = ' ';
            break;

        case 'o':
            str = number(str, va_arg(args, unsigned long), 8,
                field_width, precision, flags);
            break;

        case 'p':
            if (field_width == -1) {
                field_width = 8;
                flags |= ZEROPAD;
            }
            str = number(str,
                (unsigned long) va_arg(args, void *), 16,
                field_width, precision, flags);
            break;

        case 'x':
            flags |= SMALL;
        case 'X':
            str = number(str, va_arg(args, unsigned long), 16,
                field_width, precision, flags);
            break;

        case 'd':
        case 'i':
            flags |= SIGN;
        case 'u':
            str = number(str, va_arg(args, unsigned long), 10,
                field_width, precision, flags);
            break;

        case 'n':
            ip = va_arg(args, int *);
            *ip = (str - buf);
            break;

        default:
            if (*fmt != '%')
                *str++ = '%';
            if (*fmt)
                *str++ = *fmt;
            else
                --fmt;
            break;
        }
    }
    *str = '\0';
    return str-buf;
}
