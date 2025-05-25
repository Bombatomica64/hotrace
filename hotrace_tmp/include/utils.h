#ifndef UTILS_H
#define UTILS_H

#include "hotrace.h"
#include <stddef.h>
#include <stdint.h>

void            *ft_memcpy(void *dst, const void *src, size_t len);
int				ft_strcmp(const char *s1, const char *s2);
void			*ft_memchr(const void *s, int c, size_t n);
void            double_buff(t_ht *ht);
#endif