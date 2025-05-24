/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lmicheli <lmicheli@student.42firenze.it>   +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/24 09:57:55 by lmicheli          #+#    #+#             */
/*   Updated: 2025/05/24 09:59:57 by lmicheli         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#define TABLE_SIZE 100000

#include <unistd.h>

#define _GNU_SOURCE
#include <unistd.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <sys/stat.h>
#include <fcntl.h>

// --- HASH TABLE ---

typedef struct
{
	const char *key;
	const char *val;
} entry_t;

typedef struct
{
	entry_t *tbl;
	size_t cap;
	size_t size;
} ht_t;


#include <stdint.h>
#include <stddef.h>

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

// potenza di 2 >= x
static size_t next_pow2(size_t x)
{
	size_t p = 1;
	while (p < x)
		p <<= 1;
	return p;
}

// FNV-1a 64-bit
static inline uint64_t fnv1a(const char *s)
{
	uint64_t h = 14695981039346656037ULL;
	for (; *s; s++)
		h = (h ^ (unsigned char)*s) * 1099511628211ULL;
	return h;
}

ht_t ht_create(size_t n)
{
	ht_t ht;
	ht.cap = next_pow2(n);
	ht.size = 0;
	ht.tbl = calloc(ht.cap, sizeof(entry_t));
	return ht;
}

void ht_insert(ht_t *ht, const char *k, const char *v)
{
	uint64_t h = fnv1a(k);
	size_t i = h & (ht->cap - 1);
	// linear probing
	while (ht->tbl[i].key)
	{
		if (ht->tbl[i].key == k)
		{
			ht->tbl[i].val = v;
			return;
		}
		i = (i + 1) & (ht->cap - 1);
	}
	ht->tbl[i].key = k;
	ht->tbl[i].val = v;
	ht->size++;
}

const char *ht_get(ht_t *ht, const char *k)
{
	uint64_t h = fnv1a(k);
	size_t i = h & (ht->cap - 1);
	while (ht->tbl[i].key)
	{
		if (ht->tbl[i].key == k)
			return ht->tbl[i].val;
		i = (i + 1) & (ht->cap - 1);
	}
	return NULL;
}

// --- MAIN ---

#define BUF_SIZE (64 << 20)  // 64 MB buffer

int main(void)
{
	// 1) mmap-like read all stdin
	char *buf = malloc(BUF_SIZE + 1);
	ssize_t n = read(0, buf, BUF_SIZE);
	if (n <= 0)
		return 1;
	buf[n] = '\0';

	// 2) conta coppie per dimensionare la tabella
	size_t lines = 0;
	for (char *p = buf; p < buf + n; ++p)
		if (*p == '\n')
			lines++;
	size_t pairs = lines / 2;
	ht_t ht = ht_create(pairs * 2);

	// 3) parsing inserimenti (key\nvalue\n)
	char *p = buf, *q;
	for (size_t i = 0; i < pairs; i++)
	{
		q = strchr(p, '\n');
		*q = '\0';
		const char *key = p;
		p = q + 1;
		q = strchr(p, '\n');
		*q = '\0';
		const char *val = p;
		p = q + 1;
		ht_insert(&ht, key, val);
	}
	// salta eventuale linea vuota
	if (*p == '\n')
		p++;

	// 4) parsing lookup (key\n)
	while (p < buf + n && *p)
	{
		q = strchr(p, '\n');
		if (!q)
			break;
		*q = '\0';
		const char *key = p;
		const char *res = ht_get(&ht, key);
		if (res)
		{
			write(1, res, strlen(res));
			write(1, "\n", 1);
		}
		else
		{
			write(1, key, strlen(key));
			write(1, ": Not found\n", 12);
		}
		p = q + 1;
	}
	free(buf);
	free(n);
	return 0;
}
