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

#include "../include/utils.h"
#include "../include/hotrace.h"

void	*almemcpy(void *dst, const void *src, size_t len)
{
	size_t					i;
	unsigned long int		*longword_ptr;
	unsigned long int		longword;
	const unsigned long int	*s;

	i = 0;
	if ((uintptr_t)dst % sizeof(long) == 0 \
		&& (uintptr_t)src % sizeof(long) == 0 \
		&& len % sizeof(long) == 0)
	{
		longword_ptr = (unsigned long int *)dst;
		s = (unsigned long int *)src;
		while (i < len / sizeof(unsigned long int))
		{
			longword_ptr[i] = s[i];
			longword = longword_ptr[i];
			if (((longword - LOMAGIC) & ~longword & HIMAGIC) != 0)
				return (dst);
			i++;
		}
	}
	else
		ft_memcpy(dst, src, len);
	return (dst);
}

void	*ft_memcpy(void *dst, const void *src, size_t len)
{
	size_t		i;
	char		*d;
	const char	*s;

	i = 0;
	d = dst;
	s = src;
	while (i < len)
	{
		d[i] = s[i];
		if (d[i] == 0)
			return (dst);
		i++;
	}
	d[i] = 0;
	return (dst);
}

static void	double_buff_loop(t_ht *ht, char *k, char *v)
{
	size_t	k_off;
	size_t	v_off;
	size_t	i;

	i = -1;
	k_off = 0;
	v_off = 0;
	while (++i < ht->cap)
	{
		if (!ht->tbl[i].k)
			continue ;
		*((size_t *)((k) + k_off)) = (*((size_t *)ht->tbl[i].k - 1));
		k_off += sizeof(size_t);
		almemcpy(&k[k_off], ht->tbl[i].k, (*((size_t *)ht->tbl[i].k - 1)) + 1);
		ht->tbl[i].k = &k[k_off];
		k_off += (*((size_t *)ht->tbl[i].k - 1)) + 1;
		*((size_t *)((v) + v_off)) = (*((size_t *)ht->tbl[i].v - 1));
		v_off += sizeof(size_t);
		almemcpy(&v[v_off], ht->tbl[i].v, (*((size_t *)ht->tbl[i].v - 1)) + 1);
		ht->tbl[i].v = &v[v_off];
		v_off += (*((size_t *)ht->tbl[i].v - 1)) + 1;
	}
	ht->keys_size = k_off;
	ht->values_size = v_off;
}

void	double_buff(t_ht *ht)
{
	char	*new_keys;
	char	*new_values;

	ht->data_cap *= 2;
	new_keys = malloc(ht->data_cap);
	new_values = malloc(ht->data_cap);
	double_buff_loop(ht, new_keys, new_values);
	free(ht->keys);
	free(ht->values);
	ht->keys = new_keys;
	ht->values = new_values;
}

int	ft_strcmp(const char *s1, const char *s2)
{
	int	result;

	__asm__ volatile (
		// Save registers we'll modify
		"push %%rcx\n"
		// Main comparison loop
		"1:\n"
		"movzbl (%0), %%eax\n" // Load byte from s1 into eax
		"movzbl (%1), %%ecx\n" // Load byte from s2 into ecx
		"incq %0\n"// Increment s1 pointer
		"incq %1\n"// Increment s2 pointer
		"testb %%al, %%al\n"// Check if we reached end of s1
		"je 2f\n"// If zero, exit loop
		"cmpb %%cl, %%al\n"// Compare bytes
		"je 1b\n"// If equal, continue loop
		// Exit point - compute difference
		"2:\n"
		"subl %%ecx, %%eax\n"// Compute difference
		"pop %%rcx\n"// Restore rcx
		: "=&S"(s1), "=&D"(s2), "=&a"(result)
		: "0"(s1), "1"(s2)
		: "memory", "cc");
	return (result);
}
