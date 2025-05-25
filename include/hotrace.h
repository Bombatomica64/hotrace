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
# define BUF_SIZE 67108864

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

# if defined(__x86_64__)
#  define IS_X86_64 1
# else
#  define IS_X86_64 0
# endif

# define C1 0x87c37b91114253d5ULL
# define C2 0x4cf5ad432745937fULL
# define CHUNK_SIZE 1048576

typedef enum parse_state_e
{
	STATE_KEY,
	STATE_VALUE,
	STATE_SEARCH,
}	t_parse_state;

typedef struct s_entry
{
	void	*k;
	void	*v;
}			t_entry;

typedef struct s_ht
{
	t_entry		*tbl;
	void		*keys;
	void		*values;
	size_t		cap;
	size_t		size;
	size_t		keys_size;
	size_t		values_size;
	size_t		data_cap;
}				t_ht;

typedef struct s_murmur_ctx
{
	const void	*data;
	size_t		nbytes;
	uint64_t	h1;
	uint64_t	h2;
	uint64_t	*blocks;
	uint8_t		*tail;
	int			nblocks;
}				t_murmur_ctx;

t_ht	ht_create(size_t n);
void	ht_insert(t_ht *ht, void *k, void *v);
void	*ht_get(t_ht *ht, const char *k, size_t len);
void	ht_resize(t_ht *ht, size_t new_cap);
size_t	next_pow2(size_t n);
bool	qhashmurmur3_128(const void *data, size_t nbytes, t_murmur_ctx *ctx);
void	ht_free(t_ht *ht);
void	get_hash_values(const void *key, size_t len, uint64_t *h1,
			uint64_t *h2);
void	process_tail(const uint8_t *tail, size_t nbytes, uint64_t *h1,
			uint64_t *h2);

#endif
