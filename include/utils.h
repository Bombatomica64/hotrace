/* ************************************************************************** */
/*	                                                                          */
/*                                                        :::      ::::::::   */
/*   table.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lmicheli <lmicheli@student.42firenze.it>   +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/24 14:56:30 by lmicheli          #+#    #+#             */
/*   Updated: 2025/05/24 14:56:56 by lmicheli         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef UTILS_H
# define UTILS_H

# include "hotrace.h"
# include <stddef.h>
# include <stdint.h>

# if __GNUC__
#  if __x86_64__ || __ppc64__
#   define ENVIRONMENT64
#   define HIMAGIC -9187201950435737472L
#   define LOMAGIC 1103823438081L
#  else
#   define HIMAGIC 0x80808080L
#   define LOMAGIC 0x01010101L
#  endif
# endif

void			*almemcpy(void *dst, const void *src, size_t len);
void			*ft_memcpy(void *dst, const void *src, size_t len);
int				ft_strcmp(const char *s1, const char *s2);
void			*ft_memchr(const void *s, int c, size_t n);
void			double_buff(t_ht *ht);
#endif