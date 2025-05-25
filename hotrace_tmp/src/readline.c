#include "../include/readline.h"
#include "../include/utils.h"

int gnl(struct s_readline *rl)
{
    rl->bytes_read = read(0, rl->chunk, CHUNK_SIZE);
    if (rl->bytes_read > 0)
    {
        rl->chunk[rl->bytes_read] = '\0';
        rl->buf_pos = 0;
        return 1;
    }
    return 0;
}

int next_line(struct s_readline *rl)
{
    char *newline;
    while (rl->buf_pos < rl->bytes_read)
    {
        newline = memchr(rl->chunk + rl->buf_pos, '\n', rl->bytes_read - rl->buf_pos);
        if (newline)
        {
            ft_memcpy(rl->line_buf + rl->line_len, rl->chunk + rl->buf_pos, newline - (rl->chunk + rl->buf_pos));
            rl->line_len += newline - (rl->chunk + rl->buf_pos);
            rl->line_buf[rl->line_len] = '\0';
            rl->buf_pos += newline - (rl->chunk + rl->buf_pos) + 1;
            return 1;
        }
        else
        {
            ft_memcpy(rl->line_buf + rl->line_len,
                rl->chunk + rl->buf_pos, rl->bytes_read - rl->buf_pos);
                rl->line_len += rl->bytes_read - rl->buf_pos;
            return 0;
        }
    }
    return 0;
}

size_t get_len(struct s_readline *rl)
{
    return rl->line_len;
}

size_t clear_len(struct s_readline *rl)
{
    return rl->line_len = 0;
}

char *get_buf(struct s_readline *rl)
{
    return rl->line_buf;
}