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

#include "hotrace.h"


uint64_t murmur3_64(const void *key, size_t len, uint64_t seed) {
    const uint64_t m = 0xc6a4a7935bd1e995ULL;
    const int r = 47;

    uint64_t h = seed ^ (len * m);
    const uint64_t *data = (const uint64_t *)key;
    const uint64_t *end = data + (len / 8);

    while (data != end) {
        uint64_t k = *data++;
        k *= m;
        k ^= k >> r;
        k *= m;

        h ^= k;
        h *= m;
    }

    const unsigned char *data2 = (const unsigned char*)data;
    switch (len & 7) {
        case 7: h ^= (uint64_t)(data2[6]) << 48;
        case 6: h ^= (uint64_t)(data2[5]) << 40;
        case 5: h ^= (uint64_t)(data2[4]) << 32;
        case 4: h ^= (uint64_t)(data2[3]) << 24;
        case 3: h ^= (uint64_t)(data2[2]) << 16;
        case 2: h ^= (uint64_t)(data2[1]) << 8;
        case 1: h ^= (uint64_t)(data2[0]);
                h *= m;
    };

    h ^= h >> r;
    h *= m;
    h ^= h >> r;

    return h;
}


/**
 * Get 128-bit Murmur3 hash.
 *
 * @param data      source data
 * @param nbytes    size of data
 * @param retbuf    user buffer. It must be at leat 16-bytes long.
 *
 * @return true if successful, otherwise false.
 *
 * @code
 *   // get 128-bit Murmur3 hash.
 *   unsigned char hash[16];
 *   qhashmurmur3_128((void*)"hello", 5, hash);
 *
 *   // hex encode
 *   char *ascii = qhex_encode(hash, 16);
 *   printf("Hex encoded Murmur3: %s\n", ascii);
 *   free(ascii);
 * @endcode
 */
bool	qhashmurmur3_128(const void *data, size_t nbytes, void *retbuf) {
    if (data == NULL || nbytes == 0)
        return false;

    const uint64_t c1 = 0x87c37b91114253d5ULL;
    const uint64_t c2 = 0x4cf5ad432745937fULL;

    const int nblocks = nbytes / 16;
    const uint64_t *blocks = (const uint64_t *) (data);
    const uint8_t *tail = (const uint8_t *) (data + (nblocks * 16));

    uint64_t h1 = 0;
    uint64_t h2 = 0;

    int i;
    uint64_t k1, k2;
    for (i = 0; i < nblocks; i++) {
        k1 = blocks[i * 2 + 0];
        k2 = blocks[i * 2 + 1];

        k1 *= c1;
        k1 = (k1 << 31) | (k1 >> (64 - 31));
        k1 *= c2;
        h1 ^= k1;

        h1 = (h1 << 27) | (h1 >> (64 - 27));
        h1 += h2;
        h1 = h1 * 5 + 0x52dce729;

        k2 *= c2;
        k2 = (k2 << 33) | (k2 >> (64 - 33));
        k2 *= c1;
        h2 ^= k2;

        h2 = (h2 << 31) | (h2 >> (64 - 31));
        h2 += h1;
        h2 = h2 * 5 + 0x38495ab5;
    }

    k1 = k2 = 0;
    switch (nbytes & 15) {
        case 15:
            k2 ^= (uint64_t)(tail[14]) << 48;
        case 14:
            k2 ^= (uint64_t)(tail[13]) << 40;
        case 13:
            k2 ^= (uint64_t)(tail[12]) << 32;
        case 12:
            k2 ^= (uint64_t)(tail[11]) << 24;
        case 11:
            k2 ^= (uint64_t)(tail[10]) << 16;
        case 10:
            k2 ^= (uint64_t)(tail[9]) << 8;
        case 9:
            k2 ^= (uint64_t)(tail[8]) << 0;
            k2 *= c2;
            k2 = (k2 << 33) | (k2 >> (64 - 33));
            k2 *= c1;
            h2 ^= k2;

        case 8:
            k1 ^= (uint64_t)(tail[7]) << 56;
        case 7:
            k1 ^= (uint64_t)(tail[6]) << 48;
        case 6:
            k1 ^= (uint64_t)(tail[5]) << 40;
        case 5:
            k1 ^= (uint64_t)(tail[4]) << 32;
        case 4:
            k1 ^= (uint64_t)(tail[3]) << 24;
        case 3:
            k1 ^= (uint64_t)(tail[2]) << 16;
        case 2:
            k1 ^= (uint64_t)(tail[1]) << 8;
        case 1:
            k1 ^= (uint64_t)(tail[0]) << 0;
            k1 *= c1;
            k1 = (k1 << 31) | (k1 >> (64 - 31));
            k1 *= c2;
            h1 ^= k1;
    };

    //----------
    // finalization

    h1 ^= nbytes;
    h2 ^= nbytes;

    h1 += h2;
    h2 += h1;

    h1 ^= h1 >> 33;
    h1 *= 0xff51afd7ed558ccdULL;
    h1 ^= h1 >> 33;
    h1 *= 0xc4ceb9fe1a85ec53ULL;
    h1 ^= h1 >> 33;

    h2 ^= h2 >> 33;
    h2 *= 0xff51afd7ed558ccdULL;
    h2 ^= h2 >> 33;
    h2 *= 0xc4ceb9fe1a85ec53ULL;
    h2 ^= h2 >> 33;

    h1 += h2;
    h2 += h1;

    ((uint64_t *) retbuf)[0] = h1;
    ((uint64_t *) retbuf)[1] = h2;

    return true;
}

void ht_insert(t_ht *ht, const char *k, const char *v)
{
    uint64_t hash_result[2];
    size_t len = strlen(k);
    
    // Get both hash values in a single call
    qhashmurmur3_128(k, len, hash_result);
    uint64_t h1 = hash_result[0];
    uint64_t h2 = hash_result[1];
    
    size_t i = h1 & (ht->cap - 1);
    // Double hashing - use second hash from 128-bit result
    size_t step = 1 + (h2 & (ht->cap - 2)); // Ensure step is never 0
    
    size_t probe = 0;
    while (ht->tbl[i].key)
    {
        if (strcmp(ht->tbl[i].key, k) == 0)
        {
            ht->tbl[i].val = v;
            return;
        }
        probe++;
        i = (h1 + probe * step) & (ht->cap - 1);
    }
    ht->tbl[i].key = strdup(k);
    ht->tbl[i].val = v;
    ht->size++;
    
    // Resize if load factor exceeds 0.7
    if (ht->size > (ht->cap * 0.7))
        ht_resize(ht, ht->cap * 2);
}

const char *ht_get(t_ht *ht, const char *k)
{
    uint64_t hash_result[2];
    size_t len = strlen(k);
    
    // Get both hash values in a single call
    qhashmurmur3_128(k, len, hash_result);
    uint64_t h1 = hash_result[0];
    uint64_t h2 = hash_result[1];
    
    size_t i = h1 & (ht->cap - 1);
    size_t step = 1 + (h2 & (ht->cap - 2)); // Ensure step is never 0
    
    size_t probe = 0;
    while (ht->tbl[i].key)
    {
        if (strcmp(ht->tbl[i].key, k) == 0)
            return ht->tbl[i].val;
        
        probe++;
        i = (h1 + probe * step) & (ht->cap - 1);
    }
    return NULL;
}

// Add this resize function to handle growth
void ht_resize(t_ht *ht, size_t new_cap)
{
    t_entry *old_tbl = ht->tbl;
    size_t old_cap = ht->cap;
    
    ht->tbl = calloc(new_cap, sizeof(t_entry));
    ht->cap = new_cap;
    ht->size = 0;
    
    for (size_t i = 0; i < old_cap; i++) {
        if (old_tbl[i].key) {
            ht_insert(ht, old_tbl[i].key, old_tbl[i].val);
            free((void*)old_tbl[i].key); // Free the duplicated key
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
    // Use a good initial size (power of 2 for efficient modulo)
    ht.cap = next_pow2(n < 16 ? 16 : n);
    ht.size = 0;
    ht.tbl = calloc(ht.cap, sizeof(t_entry));
    return ht;
}

void ht_free(t_ht *ht)
{
	for (size_t i = 0; i < ht->cap; i++) {
		free((void*)ht->tbl[i].key); // Free the duplicated key
	}
	free(ht->tbl);
	ht->tbl = NULL;
}