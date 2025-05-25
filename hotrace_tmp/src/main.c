#include "../include/string.h"
#include "../include/utils.h"
#include "../include/hotrace.h"
#include "../include/readline.h"

#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>

static void search(t_ht *ht, char *line_buf, size_t line_len)
{
	void *result = ht_get(ht, line_buf, line_len);
	if (result) {
		write(1, (char *)result, *((size_t *)result - (size_t)1));
		write(1, "\n", 1);
	} else {
		write(1, line_buf, strlen(line_buf));
		write(1, ": Not found\n", 12);
	}
}

static void states(parse_state_t state, t_ht *ht, char *line_buf, size_t line_len)
{
	static char *last_key;

	if (state == STATE_KEY)
	{
		if (ht->keys_size + line_len + sizeof(size_t) > ht->data_cap \
			|| ht->values_size + (line_len) + sizeof(size_t) > ht->data_cap)
			double_buff(ht);
		*((size_t *)((ht->keys) + ht->keys_size)) = line_len;
		ht->keys_size += sizeof(size_t);
		ft_memcpy(&ht->keys[ht->keys_size], line_buf, line_len + 1);
		last_key = &ht->keys[ht->keys_size];
		ht->keys_size += ALIGN(line_len + 1);
		return ;
	}
	*((size_t *)((ht->values) + ht->values_size)) = line_len;
	ht->values_size += sizeof(size_t);
	ft_memcpy(&ht->values[ht->values_size], line_buf, line_len + 1);
	ht_insert(ht, last_key, &ht->values[ht->values_size]);
	ht->values_size += ALIGN(line_len + 1);
}

void loop(t_ht *ht)
{
	t_readline rl;
	parse_state_t state;

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
			clear_len(&rl);
		}
	}
	if (get_len(&rl) > 0 && state == STATE_SEARCH)
		search(ht, get_buf(&rl), get_len(&rl));
}

int	main(void)
{
	t_ht	ht;

	ht = ht_create(1024);
	loop(&ht);
	ht_free(&ht);
	return (0);
}
