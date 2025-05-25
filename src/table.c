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

#include "../include/hotrace.h"
#include "../include/utils.h"

void	ht_insert(t_ht *ht, void *k, void *v)
{
	t_murmur_ctx	ctx;
	size_t			i;
	size_t			step;
	size_t			probe;

	qhashmurmur3_128(k, *((size_t *)k - 1), &ctx);
	i = ctx.h1 & (ht->cap - 1);
	step = 1 + (ctx.h2 & (ht->cap - 2));
	probe = 0;
	while ((char *)ht->tbl[i].k)
	{
		if (*((size_t *)k - 1) != *((size_t *)ht->tbl[i].k - 1) || \
		ft_strcmp((char *)ht->tbl[i].k, k) == 0)
		{
			ht->tbl[i].v = v;
			return ;
		}
		probe++;
		i = (ctx.h1 + probe * step) & (ht->cap - 1);
	}
	ht->tbl[i].k = k;
	ht->tbl[i].v = v;
	ht->size++;
	if (ht->size > (ht->cap * 0.7))
		ht_resize(ht, ht->cap * 2);
}

void	*ht_get(t_ht *ht, const char *k, size_t len)
{
	t_murmur_ctx	ctx;
	size_t			i;
	size_t			step;
	size_t			probe;

	qhashmurmur3_128(k, len, &ctx);
	i = ctx.h1 & (ht->cap - 1);
	step = 1 + (ctx.h2 & (ht->cap - 2));
	probe = 0;
	while ((char *)ht->tbl[i].k)
	{
		if (len != *((size_t *)ht->tbl[i].k - 1) || \
			ft_strcmp(ht->tbl[i].k, k) == 0)
			return (ht->tbl[i].v);
		probe++;
		i = (ctx.h1 + probe * step) & (ht->cap - 1);
	}
	return (NULL);
}

void	ht_resize(t_ht *ht, size_t new_cap)
{
	t_entry	*old_tbl;
	size_t	old_cap;
	size_t	i;

	i = 0;
	old_cap = ht->cap;
	old_tbl = ht->tbl;
	ht->tbl = calloc(new_cap, sizeof(t_entry));
	ht->cap = new_cap;
	ht->size = 0;
	while (i < old_cap)
	{
		if (old_tbl[i].k)
			ht_insert(ht, old_tbl[i].k, old_tbl[i].v);
		i++;
	}
	free(old_tbl);
}

t_ht	ht_create(size_t n)
{
	t_ht	ht;

	ht.cap = next_pow2(n);
	ht.size = 0;
	ht.tbl = calloc(ht.cap, sizeof(t_entry));
	ht.data_cap = 4096 * 4096;
	ht.keys = malloc(ht.data_cap);
	ht.values = malloc(ht.data_cap);
	ht.keys_size = 0;
	ht.values_size = 0;
	return (ht);
}

void	ht_free(t_ht *ht)
{
	free(ht->keys);
	free(ht->values);
	free(ht->tbl);
	ht->tbl = NULL;
}
