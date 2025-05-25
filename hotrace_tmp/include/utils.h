#ifndef UTILS_H
#define UTILS_H

#include "hotrace.h"
#include <stddef.h>
#include <stdint.h>

// Check windows
#if _WIN32 || _WIN64
#if _WIN64
#define HIMAGIC (0x80808080L << 16) << 16 | 0x80808080L
#define LOMAGIC (0x01010101L << 16) | 0x01010101L
#else
#define HIMAGIC 0x80808080L
#define LOMAGIC 0x01010101L
#endif
#endif

// Check GCC
#if __GNUC__
#if __x86_64__ || __ppc64__
#define ENVIRONMENT64
#define HIMAGIC (0x80808080L << 16) << 16 | 0x80808080L
#define LOMAGIC (0x01010101L << 16) | 0x01010101L
#else
#define HIMAGIC 0x80808080L
#define LOMAGIC 0x01010101L
#endif
#endif

void			*ft_align_memcpy(void *dst, const void *src, size_t len);
void            *ft_memcpy(void *dst, const void *src, size_t len);
int				ft_strcmp(const char *s1, const char *s2);
void			*ft_memchr(const void *s, int c, size_t n);
void            double_buff(t_ht *ht);
#endif