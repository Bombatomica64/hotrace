/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   hotrace.h                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lmicheli <lmicheli@student.42firenze.it>   +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/24 11:07:28 by lmicheli          #+#    #+#             */
/*   Updated: 2025/05/24 13:59:11 by lmicheli         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef HOTRACE_H
# define HOTRACE_H

# define TABLE_SIZE 100000
# define BUF_SIZE (64 << 20) // 64 MB buffer

# include <unistd.h>

# define _GNU_SOURCE
# include <fcntl.h>
# include <stdbool.h>
# include <stddef.h>
# include <stdint.h>
# include <stdio.h>
# include <stdlib.h>
# include <string.h>
# include <sys/stat.h>
# include <unistd.h>

# if defined(__x86_64__) || defined(_M_X64)
#  define IS_X86_64 1
# else
#  define IS_X86_64 0
# endif

// --- HASH TABLE ---

typedef struct value_s {
    char *str;
    size_t size;
}   sstring;

typedef struct s_entry {
    sstring k;
    sstring v;
}   t_entry;

typedef struct s_ht
{
	t_entry		*tbl;
	size_t		cap;
	size_t		size;
}				t_ht;

uint64_t		murmur3_64(const void *key, size_t len, uint64_t seed);
t_ht			ht_create(size_t n);
void            ht_insert(t_ht *ht, char *k, size_t key_size, char *v, size_t value_size);
sstring         ht_get(t_ht *ht, const char *k, size_t len);
void			ht_resize(t_ht *ht, size_t new_cap);
size_t			next_pow2(size_t n);
bool			qhashmurmur3_128(const void *data, size_t nbytes, void *retbuf);
void			ht_free(t_ht *ht);

#endif
