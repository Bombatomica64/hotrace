#ifndef READLINE_H
#define READLINE_H

#include <stddef.h>
#include <unistd.h>
#define CHUNK_SIZE (1 << 20)

typedef struct s_readline {
    char chunk[CHUNK_SIZE + 1];
    char line_buf[CHUNK_SIZE];
    size_t line_len;
    ssize_t bytes_read;
    ssize_t buf_pos;
}   t_readline;

int gnl(struct s_readline *rl);
int next_line(struct s_readline *rl);
size_t get_len(struct s_readline *rl);
char *get_buf(struct s_readline *rl);
size_t clear_len(struct s_readline *rl);

#endif