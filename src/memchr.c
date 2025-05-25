/* ************************************************************************** */
/*	                                                                        */
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

static void	*ft_memchr_small(const unsigned char *src, unsigned char uc,
	size_t n)
{
	while (n--)
	{
		if (*src == uc)
			return ((void *)src);
		src++;
	}
	return (NULL);
}

static void	ft_broadcast_char(unsigned char uc)
{
	__asm__ (
		"movd %0, %%xmm1\n"
		"punpcklbw %%xmm1, %%xmm1\n"
		"punpcklwd %%xmm1, %%xmm1\n"
		"pshufd $0, %%xmm1, %%xmm1\n"
		:
		: "r"((uint32_t)uc)
		: "xmm1"
		);
}

static void	*ft_memchr_avx2(const unsigned char *src, size_t *n)
{
	uint32_t	mask;

	while (*n >= 32)
	{
		__asm__ (
			"vmovdqu (%1), %%ymm0\n"
			"vpcmpeqb %%ymm1, %%ymm0, %%ymm0\n"
			"vpmovmskb %%ymm0, %0\n"
			: "=r"(mask)
			: "r"(src)
			: "ymm0"
			);
		if (mask)
			return ((void *)(src + __builtin_ctz(mask)));
		src += 32;
		*n -= 32;
	}
	return ((void *)src);
}

void	*ft_memchr(const void *s, int c, size_t n)
{
	const unsigned char	*src;
	unsigned char		uc;
	const unsigned char	*end_ptr;
	void				*result;

	src = s;
	uc = c;
	if (n < 32)
		return (ft_memchr_small(src, uc, n));
	ft_broadcast_char(uc);
	end_ptr = ft_memchr_avx2(src, &n);
	if (end_ptr != src)
		src = end_ptr;
	result = ft_memchr_small(src, uc, n);
	__asm__ volatile ("vzeroupper" ::: "ymm0", "ymm1");
	return (result);
}
