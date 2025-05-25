/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   hash2.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lmicheli <lmicheli@student.42firenze.it>   +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/24 16:47:34 by lmicheli          #+#    #+#             */
/*   Updated: 2025/05/24 16:47:51 by lmicheli         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <hotrace.h>

void	process_tail_k2(const uint8_t *tail, int res, uint64_t *h2,
		uint64_t *k2)
{
	if (res == 15)
		*k2 ^= (uint64_t)(tail[14]) << 48;
	if (res >= 14)
		*k2 ^= (uint64_t)(tail[13]) << 40;
	if (res >= 13)
		*k2 ^= (uint64_t)(tail[12]) << 32;
	if (res >= 12)
		*k2 ^= (uint64_t)(tail[11]) << 24;
	if (res >= 11)
		*k2 ^= (uint64_t)(tail[10]) << 16;
	if (res >= 10)
		*k2 ^= (uint64_t)(tail[9]) << 8;
	if (res >= 9)
	{
		*k2 ^= (uint64_t)(tail[8]) << 0;
		*k2 *= C2;
		*k2 = (*k2 << 33) | (*k2 >> (64 - 33));
		*k2 *= C1;
		*h2 ^= *k2;
	}
}

void	process_tail_k1(const uint8_t *tail, int res, uint64_t *h1,
		uint64_t *k1)
{
	if (res == 8)
		*k1 ^= (uint64_t)(tail[7]) << 56;
	if (res >= 7)
		*k1 ^= (uint64_t)(tail[6]) << 48;
	if (res >= 6)
		*k1 ^= (uint64_t)(tail[5]) << 40;
	if (res >= 5)
		*k1 ^= (uint64_t)(tail[4]) << 32;
	if (res >= 4)
		*k1 ^= (uint64_t)(tail[3]) << 24;
	if (res >= 3)
		*k1 ^= (uint64_t)(tail[2]) << 16;
	if (res >= 2)
		*k1 ^= (uint64_t)(tail[1]) << 8;
	if (res >= 1)
	{
		*k1 ^= (uint64_t)(tail[0]) << 0;
		*k1 *= C1;
		*k1 = (*k1 << 31) | (*k1 >> (64 - 31));
		*k1 *= C2;
		*h1 ^= *k1;
	}
}

void	process_tail(const uint8_t *tail, size_t nbytes, uint64_t *h1,
		uint64_t *h2)
{
	int			res;
	uint64_t	k1;
	uint64_t	k2;

	k1 = 0;
	k2 = 0;
	res = nbytes & 15;
	if (res > 8)
		process_tail_k2(tail, res, h2, &k2);
	else
		process_tail_k1(tail, res, h1, &k1);
}
