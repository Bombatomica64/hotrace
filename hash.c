/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   hash.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lmicheli <lmicheli@student.42firenze.it>   +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/24 11:07:16 by lmicheli          #+#    #+#             */
/*   Updated: 2025/05/24 11:08:47 by lmicheli         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <hotrace.h>

// ==================== MurmurHash Implementation ====================

static void	finalize_hash(size_t nbytes, uint64_t *h1, uint64_t *h2,
		void *retbuf)
{
	*h1 ^= nbytes;
	*h2 ^= nbytes;
	*h1 += *h2;
	*h2 += *h1;
	*h1 ^= *h1 >> 33;
	*h1 *= 0xff51afd7ed558ccdULL;
	*h1 ^= *h1 >> 33;
	*h1 *= 0xc4ceb9fe1a85ec53ULL;
	*h1 ^= *h1 >> 33;
	*h2 ^= *h2 >> 33;
	*h2 *= 0xff51afd7ed558ccdULL;
	*h2 ^= *h2 >> 33;
	*h2 *= 0xc4ceb9fe1a85ec53ULL;
	*h2 ^= *h2 >> 33;
	*h1 += *h2;
	*h2 += *h1;
	((uint64_t *)retbuf)[0] = *h1;
	((uint64_t *)retbuf)[1] = *h2;
}

static void	init_murmur_ctx(t_murmur_ctx *ctx, const void *data, size_t nbytes)
{
	ctx->data = data;
	ctx->nbytes = nbytes;
	ctx->nblocks = nbytes / 16;
	ctx->blocks = (uint64_t *)data;
	ctx->tail = (uint8_t *)(data + (ctx->nblocks * 16));
	ctx->h1 = 0;
	ctx->h2 = 0;
}

static void	process_blocks(t_murmur_ctx *ctx)
{
	int			i;
	uint64_t	k1;
	uint64_t	k2;

	i = 0;
	while (ctx->nblocks-- > 0)
	{
		k1 = ctx->blocks[i * 2 + 0];
		k2 = ctx->blocks[i * 2 + 1];
		k1 *= C1;
		k1 = (k1 << 31) | (k1 >> (64 - 31));
		k1 *= C2;
		ctx->h1 ^= k1;
		ctx->h1 = (ctx->h1 << 27) | (ctx->h1 >> (64 - 27));
		ctx->h1 += ctx->h2;
		ctx->h1 = ctx->h1 * 5 + 0x52dce729;
		k2 *= C2;
		k2 = (k2 << 33) | (k2 >> (64 - 33));
		k2 *= C1;
		ctx->h2 ^= k2;
		ctx->h2 = (ctx->h2 << 31) | (ctx->h2 >> (64 - 31));
		ctx->h2 += ctx->h1;
		ctx->h2 = ctx->h2 * 5 + 0x38495ab5;
		i++;
	}
}

bool	qhashmurmur3_128(const void *data, size_t nbytes, void *retbuf)
{
	t_murmur_ctx	ctx;

	if (data == NULL || nbytes == 0)
		return (false);
	init_murmur_ctx(&ctx, data, nbytes);
	process_blocks(&ctx);
	process_tail(ctx.tail, nbytes, &ctx.h1, &ctx.h2);
	finalize_hash(nbytes, &ctx.h1, &ctx.h2, retbuf);
	return (true);
}


// bool qhashmurmur3_128(const void *data, size_t nbytes, void *retbuf)
// {
//     if (data == NULL || nbytes == 0)
//         return (false);
        
//     // FNV-1a constants
//     uint64_t hash = 0xcbf29ce484222325ULL; // FNV offset basis
//     const uint64_t fnv_prime = 0x100000001b3ULL;
    
//     // Compute FNV-1a hash
//     const uint8_t *bytes = (const uint8_t*)data;
//     for (size_t i = 0; i < nbytes; i++) {
//         hash ^= bytes[i];
//         hash *= fnv_prime;
//     }
    
//     // Generate a second 64-bit hash value by applying more mixing
//     uint64_t hash2 = hash;
//     hash2 ^= hash2 >> 33;
//     hash2 *= 0xff51afd7ed558ccdULL;
//     hash2 ^= hash2 >> 33;
//     hash2 *= 0xc4ceb9fe1a85ec53ULL;
//     hash2 ^= hash2 >> 33;
    
//     // Store both 64-bit values in the return buffer
//     ((uint64_t *)retbuf)[0] = hash;
//     ((uint64_t *)retbuf)[1] = hash2;
    
//     return (true);
// }