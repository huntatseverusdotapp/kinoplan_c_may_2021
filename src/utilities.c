// This is an open source non-commercial project. Dear PVS-Studio, please check it.

// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com

#include "utilities.h"

#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>

// Prints error message and exits
int errexit(const char* format, ...)
{
    va_list args;

    va_start(args, format);

    vfprintf(stderr, format, args);

    va_end(args);

    exit(1);
}

// Converts int32_t to c-str
char* i32toa(int32_t i, char b[]) {
    char const digit[] = "0123456789";
    char* p = b;
    if (i<0) {
        *p++ = '-';
        i *= -1;
    }
    int32_t shifter = i;
    do { // Move to where representation ends
        ++p;
        shifter = shifter/10;
    } while(shifter);
    *p = '\0';
    do { // Move back, inserting digits as u go
        int32_t d_index = i%10;
        d_index = d_index < 0? -d_index: d_index;
        *--p = digit[d_index];
        i = i/10;
    } while(i);
    return b;
}
