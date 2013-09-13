/*
 * Copyright (c) 2000-2006 Apple Computer, Inc. All rights reserved.
 *
 * @APPLE_OSREFERENCE_LICENSE_HEADER_START@
 * 
 * This file contains Original Code and/or Modifications of Original Code
 * as defined in and that are subject to the Apple Public Source License
 * Version 2.0 (the 'License'). You may not use this file except in
 * compliance with the License. The rights granted to you under the License
 * may not be used to create, or enable the creation or redistribution of,
 * unlawful or unlicensed copies of an Apple operating system, or to
 * circumvent, violate, or enable the circumvention or violation of, any
 * terms of an Apple operating system software license agreement.
 * 
 * Please obtain a copy of the License at
 * http://www.opensource.apple.com/apsl/ and read it before using this file.
 * 
 * The Original Code and all software distributed under the License are
 * distributed on an 'AS IS' basis, WITHOUT WARRANTY OF ANY KIND, EITHER
 * EXPRESS OR IMPLIED, AND APPLE HEREBY DISCLAIMS ALL SUCH WARRANTIES,
 * INCLUDING WITHOUT LIMITATION, ANY WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE, QUIET ENJOYMENT OR NON-INFRINGEMENT.
 * Please see the License for the specific language governing rights and
 * limitations under the License.
 * 
 * @APPLE_OSREFERENCE_LICENSE_HEADER_END@
 */
/*
 * @OSF_COPYRIGHT@
 */

/* 
 * Mach Operating System
 * Copyright (c) 1991,1990,1989,1988 Carnegie Mellon University
 * All Rights Reserved.
 * 
 * Permission to use, copy, modify and distribute this software and its
 * documentation is hereby granted, provided that both the copyright
 * notice and this permission notice appear in all copies of the
 * software, derivative works or modified versions, and any portions
 * thereof, and that both notices appear in supporting documentation.
 * 
 * CARNEGIE MELLON ALLOWS FREE USE OF THIS SOFTWARE IN ITS "AS IS"
 * CONDITION.  CARNEGIE MELLON DISCLAIMS ANY LIABILITY OF ANY KIND FOR
 * ANY DAMAGES WHATSOEVER RESULTING FROM THE USE OF THIS SOFTWARE.
 * 
 * Carnegie Mellon requests users of this software to return to
 * 
 *  Software Distribution Coordinator  or  Software.Distribution@CS.CMU.EDU
 *  School of Computer Science
 *  Carnegie Mellon University
 *  Pittsburgh PA 15213-3890
 * 
 * any improvements or extensions that they make and grant Carnegie Mellon
 * the rights to redistribute these changes.
 */

#include "genboot.h"

#define __unused
typedef enum { FALSE, TRUE } boolean_t;

#define isdigit(d) ((d) >= '0' && (d) <= '9')
#define Ctod(c) ((c) - '0')

#define MAXBUF (sizeof(long long int) * 8)  /* enough for binary */
static char digs[] = "0123456789abcdef";


#if CONFIG_NO_PRINTF_STRINGS
/* Prevent CPP from breaking the definition below */
#undef printf
#endif

int _consume_printf_args(int a __unused, ...)
{
    return 0;
}
void _consume_kprintf_args(int a __unused, ...)
{
}

static int
printnum(
    unsigned long long int  u,  /* number to print */
    int     base,
    void            (*putc)(int, void *),
    void                    *arg)
{
    char    buf[MAXBUF];    /* build number here */
    char *  p = &buf[MAXBUF-1];
    int nprinted = 0;

    do {
        *p-- = digs[u % base];
        u /= base;
    } while (u != 0);

    while (++p != &buf[MAXBUF]) {
        (*putc)(*p, arg);
        nprinted++;
    }

    return nprinted;
}

boolean_t   _doprnt_truncates = FALSE;

int
__doprnt(
    const char  *fmt,
    va_list         argp,
                        /* character output routine */
    void            (*putc)(int, void *arg),
    void                    *arg,
    int         radix)      /* default radix - for '%r' */
{
    int     length;
    int     prec;
    boolean_t   ladjust;
    char        padc;
    long long       n;
    unsigned long long  u;
    int     plus_sign;
    int     sign_char;
    boolean_t   altfmt, truncate;
    int     base;
    char    c;
    int     capitals;
    int     long_long;
    int             nprinted = 0;

    while ((c = *fmt) != '\0') {
        if (c != '%') {
        (*putc)(c, arg);
        nprinted++;
        fmt++;
        continue;
        }

        fmt++;

        long_long = 0;
        length = 0;
        prec = -1;
        ladjust = FALSE;
        padc = ' ';
        plus_sign = 0;
        sign_char = 0;
        altfmt = FALSE;

        while (TRUE) {
        c = *fmt;
        if (c == '#') {
            altfmt = TRUE;
        }
        else if (c == '-') {
            ladjust = TRUE;
        }
        else if (c == '+') {
            plus_sign = '+';
        }
        else if (c == ' ') {
            if (plus_sign == 0)
            plus_sign = ' ';
        }
        else
            break;
        fmt++;
        }

        if (c == '0') {
        padc = '0';
        c = *++fmt;
        }

        if (isdigit(c)) {
        while(isdigit(c)) {
            length = 10 * length + Ctod(c);
            c = *++fmt;
        }
        }
        else if (c == '*') {
        length = va_arg(argp, int);
        c = *++fmt;
        if (length < 0) {
            ladjust = !ladjust;
            length = -length;
        }
        }

        if (c == '.') {
        c = *++fmt;
        if (isdigit(c)) {
            prec = 0;
            while(isdigit(c)) {
            prec = 10 * prec + Ctod(c);
            c = *++fmt;
            }
        }
        else if (c == '*') {
            prec = va_arg(argp, int);
            c = *++fmt;
        }
        }

        if (c == 'l') {
        c = *++fmt; /* need it if sizeof(int) < sizeof(long) */
        if (sizeof(int)<sizeof(long))
            long_long = 1;
        if (c == 'l') {
            long_long = 1;
            c = *++fmt;
        }   
        } else if (c == 'q' || c == 'L') {
            long_long = 1;
        c = *++fmt;
        } 

        truncate = FALSE;
        capitals=0;     /* Assume lower case printing */

        switch(c) {
        case 'b':
        case 'B':
        {
            register char *p;
            boolean_t     any;
            register int  i;

            if (long_long) {
            u = va_arg(argp, unsigned long long);
            } else {
            u = va_arg(argp, unsigned int);
            }
            p = va_arg(argp, char *);
            base = *p++;
            nprinted += printnum(u, base, putc, arg);

            if (u == 0)
            break;

            any = FALSE;
            while ((i = *p++) != '\0') {
            if (*fmt == 'B')
                i = 33 - i;
            if (*p <= 32) {
                /*
                 * Bit field
                 */
                register int j;
                if (any)
                (*putc)(',', arg);
                else {
                (*putc)('<', arg);
                any = TRUE;
                }
                nprinted++;
                j = *p++;
                if (*fmt == 'B')
                j = 32 - j;
                for (; (c = *p) > 32; p++) {
                (*putc)(c, arg);
                nprinted++;
                }
                nprinted += printnum((unsigned)( (u>>(j-1)) & ((2<<(i-j))-1)),
                         base, putc, arg);
            }
            else if (u & (1<<(i-1))) {
                if (any)
                (*putc)(',', arg);
                else {
                (*putc)('<', arg);
                any = TRUE;
                }
                nprinted++;
                for (; (c = *p) > 32; p++) {
                (*putc)(c, arg);
                nprinted++;
                }
            }
            else {
                for (; *p > 32; p++)
                continue;
            }
            }
            if (any) {
            (*putc)('>', arg);
            nprinted++;
            }
            break;
        }

        case 'c':
            c = va_arg(argp, int);
            (*putc)(c, arg);
            nprinted++;
            break;

        case 's':
        {
            register const char *p;
            register const char *p2;

            if (prec == -1)
            prec = 0x7fffffff;  /* MAXINT */

            p = va_arg(argp, char *);

            if (p == NULL)
            p = "";

            if (length > 0 && !ladjust) {
            n = 0;
            p2 = p;

            for (; *p != '\0' && n < prec; p++)
                n++;

            p = p2;

            while (n < length) {
                (*putc)(' ', arg);
                n++;
                nprinted++;
            }
            }

            n = 0;

            while ((n < prec) && (!(length > 0 && n >= length))) {
                if (*p == '\0') {
                    break;
                }
                (*putc)(*p++, arg);
                nprinted++;
                n++;
            }

            if (n < length && ladjust) {
            while (n < length) {
                (*putc)(' ', arg);
                n++;
                nprinted++;
            }
            }

            break;
        }

        case 'o':
            truncate = _doprnt_truncates;
        case 'O':
            base = 8;
            goto print_unsigned;

        case 'D': {
            unsigned char *up;
            char *q, *p;
            
            up = (unsigned char *)va_arg(argp, unsigned char *);
            p = (char *)va_arg(argp, char *);
            if (length == -1)
                length = 16;
            while(length--) {
                (*putc)(digs[(*up >> 4)], arg);
                (*putc)(digs[(*up & 0x0f)], arg);
                nprinted += 2;
                up++;
                if (length) {
                    for (q=p;*q;q++) {
                        (*putc)(*q, arg);
                        nprinted++;
                    }
                }
            }
            break;
        }

        case 'd':
            truncate = _doprnt_truncates;
            base = 10;
            goto print_signed;

        case 'u':
            truncate = _doprnt_truncates;
        case 'U':
            base = 10;
            goto print_unsigned;

        case 'p':
            altfmt = TRUE;
            if (sizeof(int)<sizeof(void *)) {
            long_long = 1;
            }
        case 'x':
            truncate = _doprnt_truncates;
            base = 16;
            goto print_unsigned;

        case 'X':
            base = 16;
            capitals=16;    /* Print in upper case */
            goto print_unsigned;

        case 'z':
            truncate = _doprnt_truncates;
            base = 16;
            goto print_signed;
            
        case 'Z':
            base = 16;
            capitals=16;    /* Print in upper case */
            goto print_signed;

        case 'r':
            truncate = _doprnt_truncates;
        case 'R':
            base = radix;
            goto print_signed;

        case 'n':
            truncate = _doprnt_truncates;
        case 'N':
            base = radix;
            goto print_unsigned;

        print_signed:
            if (long_long) {
            n = va_arg(argp, long long);
            } else {
            n = va_arg(argp, int);
            }
            if (n >= 0) {
            u = n;
            sign_char = plus_sign;
            }
            else {
            u = -n;
            sign_char = '-';
            }
            goto print_num;

        print_unsigned:
            if (long_long) {
            u = va_arg(argp, unsigned long long);
            } else { 
            u = va_arg(argp, unsigned int);
            }
            goto print_num;

        print_num:
        {
            char    buf[MAXBUF];    /* build number here */
            register char * p = &buf[MAXBUF-1];
            static char digits[] = "0123456789abcdef0123456789ABCDEF";
            const char *prefix = NULL;

            if (truncate) u = (long long)((int)(u));

            if (u != 0 && altfmt) {
            if (base == 8)
                prefix = "0";
            else if (base == 16)
                prefix = "0x";
            }

            do {
            /* Print in the correct case */
            *p-- = digits[(u % base)+capitals];
            u /= base;
            } while (u != 0);

            length -= (int)(&buf[MAXBUF-1] - p);
            if (sign_char)
            length--;
            if (prefix)
            length -= (int)strlen(prefix);

            if (padc == ' ' && !ladjust) {
            /* blank padding goes before prefix */
            while (--length >= 0) {
                (*putc)(' ', arg);
                nprinted++;
            }               
            }
            if (sign_char) {
            (*putc)(sign_char, arg);
            nprinted++;
            }
            if (prefix) {
            while (*prefix) {
                (*putc)(*prefix++, arg);
                nprinted++;
            }
            }
            if (padc == '0') {
            /* zero padding goes after sign and prefix */
            while (--length >= 0) {
                (*putc)('0', arg);
                nprinted++;
            }               
            }
            while (++p != &buf[MAXBUF]) {
            (*putc)(*p, arg);
            nprinted++;
            }
            
            if (ladjust) {
            while (--length >= 0) {
                (*putc)(' ', arg);
                nprinted++;
            }
            }
            break;
        }

        case '\0':
            fmt--;
            break;

        default:
            (*putc)(c, arg);
            nprinted++;
        }
    fmt++;
    }

    return nprinted;
}

static void
dummy_putc(int ch, void *arg)
{
    void (*real_putc)(char) = arg;
    
    real_putc(ch);
}

void 
_doprnt(
    register const char *fmt,
    va_list         *argp,
                        /* character output routine */
    void            (*putc)(char),
    int         radix)      /* default radix - for '%r' */
{
    __doprnt(fmt, *argp, dummy_putc, putc, radix);
}

static void
copybyte(int c, void *arg)
{
    /*
     * arg is a pointer (outside pointer) to the pointer
     * (inside pointer) which points to the character.
     * We pass a double pointer, so that we can increment
     * the inside pointer.
     */
    char** p = arg; /* cast outside pointer */
    **p = c;    /* store character */
    (*p)++;     /* increment inside pointer */
}

extern void uart_putchar(char c);

void
consdebug_putc(char c)
{
    uart_putchar(c);
}

void
printf(char *fmt, ...)
{
    va_list listp;

    va_start(listp, fmt);
    _doprnt(fmt, &listp, consdebug_putc, 16);
    va_end(listp);
    return;
}

void
init_printf(void *putp, void (*putf) (void *, char))
{
    return;
}

void
vprintf(char *fmt, va_list ap)
{
    _doprnt(fmt, &ap, consdebug_putc, 16);
    return;
}

/*
 * Deprecation Warning:
 *  sprintf() is being deprecated. Please use snprintf() instead.
 */
int
sprintf(char *buf, const char *fmt, ...)
{
        va_list listp;
    char *copybyte_str;

        va_start(listp, fmt);
        copybyte_str = buf;
        __doprnt(fmt, listp, copybyte, &copybyte_str, 16);
        va_end(listp);
    *copybyte_str = '\0';
        return (int)strlen(buf);
}
