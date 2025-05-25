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

#include "../include/readline.h"
#include "../include/utils.h"

void	init_gnl(struct s_readline *rl)
{
	rl->bytes_read = 0;
	rl->line_len = 0;
	rl->buf_pos = 0;
	rl->chunk = malloc(CHUNK_SIZE + 1);
	rl->line_buf = malloc(CHUNK_SIZE);
}

int	gnl(struct s_readline *rl)
{
	rl->bytes_read = read(0, rl->chunk, CHUNK_SIZE);
	if (rl->bytes_read > 0)
	{
		rl->chunk[rl->bytes_read] = '\0';
		rl->buf_pos = 0;
		return (1);
	}
	return (0);
}

int	next_line(struct s_readline *rl)
{
	char	*newline;

	while (rl->buf_pos < rl->bytes_read)
	{
		newline = memchr(rl->chunk + rl->buf_pos,
				'\n', rl->bytes_read - rl->buf_pos);
		if (newline)
		{
			ft_memcpy(rl->line_buf + rl->line_len,
				rl->chunk + rl->buf_pos, newline - (rl->chunk + rl->buf_pos));
			rl->line_len += newline - (rl->chunk + rl->buf_pos);
			rl->line_buf[rl->line_len] = '\0';
			rl->buf_pos += newline - (rl->chunk + rl->buf_pos) + 1;
			return (1);
		}
		else
		{
			ft_memcpy(rl->line_buf + rl->line_len,
				rl->chunk + rl->buf_pos, rl->bytes_read - rl->buf_pos);
			rl->line_len += rl->bytes_read - rl->buf_pos;
			return (0);
		}
	}
	return (0);
}

size_t	get_len(struct s_readline *rl)
{
	return (rl->line_len);
}

char	*get_buf(struct s_readline *rl)
{
	return (rl->line_buf);
}
