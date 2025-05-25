/* ************************************************************************** */
/*	                                                                          */
/*                                                        :::      ::::::::   */
/*   table.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lmicheli <lmicheli@student.42firenze.it>   +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/24 14:56:30 by lmicheli          #+#    #+#             */
/*   Updated: 2025/05/24 14:56:56 by lmicheli         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef READLINE_H
# define READLINE_H
# include <stddef.h>
# include <unistd.h>
# define CHUNK_SIZE 1048576

typedef struct s_readline
{
	char			*chunk;
	char			*line_buf;
	size_t			line_len;
	ssize_t			bytes_read;
	ssize_t			buf_pos;
}					t_readline;

void	init_gnl(struct s_readline *rl);
int		gnl(struct s_readline *rl);
int		next_line(struct s_readline *rl);
size_t	get_len(struct s_readline *rl);
char	*get_buf(struct s_readline *rl);

#endif