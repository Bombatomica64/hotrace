#ifndef STRING_H
#define STRING_H

#include <stddef.h>

typedef struct string_s {
    size_t  size;
    char *buff;
}   string;

#endif