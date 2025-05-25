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

#include "../include/string.h"
#include "../include/utils.h"
#include "../include/hotrace.h"
#include "../include/readline.h"

#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

static void	search(t_ht *ht, char *line_buf, size_t line_len)
{
	void	*result;

	result = ht_get(ht, line_buf, line_len);
	if (result)
	{
		write(1, (char *)result, *((size_t *)result - (size_t)1));
		write(1, "\n", 1);
	}
	else
	{
		write(1, line_buf, strlen(line_buf));
		write(1, ": Not found\n", 12);
	}
}

static void	states(t_parse_state state, t_ht *ht, char *b, size_t line_len)
{
	static char	*last_key;

	if (state == STATE_KEY)
	{
		if (ht->keys_size + line_len + sizeof(size_t) > ht->data_cap \
			|| ht->values_size + (line_len) + sizeof(size_t) > ht->data_cap)
			double_buff(ht);
		*((size_t *)((ht->keys) + ht->keys_size)) = line_len;
		ht->keys_size += sizeof(size_t);
		almemcpy(&ht->keys[ht->keys_size], b, line_len + 1);
		last_key = &ht->keys[ht->keys_size];
		ht->keys_size += line_len + 1;
		return ;
	}
	*((size_t *)((ht->values) + ht->values_size)) = line_len;
	ht->values_size += sizeof(size_t);
	almemcpy(&ht->values[ht->values_size], b, line_len + 1);
	ht_insert(ht, last_key, &ht->values[ht->values_size]);
	ht->values_size += line_len + 1;
}

void	loop(t_ht *ht)
{
	t_readline		rl;
	t_parse_state	state;

	init_gnl(&rl);
	state = STATE_KEY;
	while (gnl(&rl))
	{
		while (next_line(&rl))
		{
			if (get_len(&rl) == 0)
				state = STATE_SEARCH;
			else if (state == STATE_KEY)
				states(state++, ht, get_buf(&rl), get_len(&rl));
			else if (state == STATE_VALUE)
				states(state--, ht, get_buf(&rl), get_len(&rl));
			else if (state == STATE_SEARCH)
				search(ht, get_buf(&rl), get_len(&rl));
			rl.line_len = 0;
		}
	}
	if (get_len(&rl) > 0 && state == STATE_SEARCH)
		search(ht, get_buf(&rl), get_len(&rl));
	free(rl.chunk);
	free(rl.line_buf);
}

int	main(void)
{
	t_ht	ht;

	ht = ht_create(4096);
	loop(&ht);
	ht_free(&ht);
	return (0);
}
