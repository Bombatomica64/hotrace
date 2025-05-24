/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   table.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lmicheli <lmicheli@student.42firenze.it>   +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/24 14:56:30 by lmicheli          #+#    #+#             */
/*   Updated: 2025/05/24 14:56:56 by lmicheli         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "hotrace.h"

void	get_hash_values(const void *key, size_t len, uint64_t *h1, uint64_t *h2)
{
	uint64_t	hash_result[2];

	qhashmurmur3_128(key, len, hash_result);
	*h1 = hash_result[0];
	*h2 = hash_result[1];
}

void            ht_insert(t_ht *ht, void *k, void *v)
{
    uint64_t hash_result[2];

    // Get both hash values in a single call
    qhashmurmur3_128(k, *((size_t *)k - 1), hash_result);
    uint64_t h1 = hash_result[0];
    uint64_t h2 = hash_result[1];
    
    size_t i = h1 & (ht->cap - 1);
    // Double hashing - use second hash from 128-bit result
    size_t step = 1 + (h2 & (ht->cap - 2)); // Ensure step is never 0
    
    size_t probe = 0;
    while ((char *)ht->tbl[i].k)
    {
        if (strcmp((char *)ht->tbl[i].k, k) == 0)
        {
            ht->tbl[i].v = v;
            return;
        }
        probe++;
        i = (h1 + probe * step) & (ht->cap - 1);
    }
    ht->tbl[i].k = k;
    ht->tbl[i].v = v;
    ht->size++;
    
    // Resize if load factor exceeds 0.7
    if (ht->size > (ht->cap * 0.7))
        ht_resize(ht, ht->cap * 2);
}

void *ht_get(t_ht *ht, const char *k, size_t len)
{
    uint64_t hash_result[2];
    // Get both hash values in a single call
    qhashmurmur3_128(k, len, hash_result);
    uint64_t h1 = hash_result[0];
    uint64_t h2 = hash_result[1];
    
    size_t i = h1 & (ht->cap - 1);
    size_t step = 1 + (h2 & (ht->cap - 2));
    
    size_t probe = 0;
    while ((char *)ht->tbl[i].k)
    {
        if (strcmp(ht->tbl[i].k, k) == 0)
            return ht->tbl[i].v;
        
        probe++;
        i = (h1 + probe * step) & (ht->cap - 1);
    }
    return NULL;
}

// void	ht_insert(t_ht *ht, const char *k, const char *v)
// {
// 	size_t	len;
// 	size_t	i;
// 	size_t	probe;

// 	uint64_t h1, h2;
// 	len = strlen(k);
// 	get_hash_values(k, len, &h1, &h2);
// 	i = h1 & (ht->cap - 1);
// 	size_t step = 1 + (h2 & (ht->cap - 2));
// 	probe = 0;
// 	while (ht->tbl[i].key)
// 	{
// 		if (strcmp(ht->tbl[i].key, k) == 0)
// 		{
// 			ht->tbl[i].val = v;
// 			return ;
// 		}
// 		probe++;
// 		i = (h1 + probe * step) & (ht->cap - 1);
// 	}
// 	ht->tbl[i].key = strdup(k);
// 	ht->tbl[i].val = v;
// 	ht->size++;
// 	// Resize if load factor exceeds 0.7
// 	if (ht->size > (ht->cap * 0.7))
// 		ht_resize(ht, ht->cap * 2);
// }

// const char	*ht_get(t_ht *ht, const char *k)
// {
// 	size_t	len;
// 	size_t	i;
// 	size_t	probe;

// 	uint64_t h1, h2;
// 	len = strlen(k);
// 	// Get hash values based on the selected hash function
// 	get_hash_values(k, len, &h1, &h2);
// 	i = h1 & (ht->cap - 1);
// 	size_t step = 1 + (h2 & (ht->cap - 2)); // Ensure step is never 0
// 	probe = 0;
// 	while (ht->tbl[i].key)
// 	{
// 		if (strcmp(ht->tbl[i].key, k) == 0)
// 			return (ht->tbl[i].val);
// 		probe++;
// 		i = (h1 + probe * step) & (ht->cap - 1);
// 	}
// 	return (NULL);
// }

void ht_resize(t_ht *ht, size_t new_cap)
{
    t_entry *old_tbl = ht->tbl;
    size_t old_cap = ht->cap;
    
    ht->tbl = calloc(new_cap, sizeof(t_entry));
    ht->cap = new_cap;
    ht->size = 0;
    
    for (size_t i = 0; i < old_cap; i++) {
        if (old_tbl[i].k) {
            ht_insert(ht, old_tbl[i].k, old_tbl[i].v);
            // free((void*)old_tbl[i].k.str); // Free the duplicated key
        }
    }
    
    free(old_tbl);
}

size_t next_pow2(size_t n) {
    n--;
    n |= n >> 1;
    n |= n >> 2;
    n |= n >> 4;
    n |= n >> 8;
    n |= n >> 16;
    n |= n >> 32;
    return n + 1;
}

t_ht ht_create(size_t n)
{
    t_ht ht;
    
    ht.cap = next_pow2(n < 16 ? 16 : n);
    ht.size = 0;
    ht.tbl = calloc(ht.cap, sizeof(t_entry));
    return ht;
}

void ht_free(t_ht *ht)
{
	free(ht->tbl);
	ht->tbl = NULL;
}