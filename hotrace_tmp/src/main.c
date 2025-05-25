#include "../include/string.h"
#include "../include/utils.h"
#include "../include/hotrace.h"

#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <string.h>

int main(void)
{
    char chunk[CHUNK_SIZE + 1];
    char line_buf[CHUNK_SIZE];
    size_t line_len = 0;
    t_ht ht = ht_create(1024);
    parse_state_t state = STATE_KEY;
    ssize_t bytes_read;
    ssize_t buf_pos = 0;

    char *last_key;
    while ((bytes_read = read(0, chunk, CHUNK_SIZE)) > 0) {
        chunk[bytes_read] = '\0';
        buf_pos = 0;
        
        while (buf_pos < bytes_read) {
            char *newline = memchr(chunk + buf_pos, '\n', bytes_read - buf_pos);
            
            if (newline) {
                size_t line_part_len = newline - (chunk + buf_pos);
                
                ft_memcpy(line_buf + line_len, chunk + buf_pos, line_part_len);
                line_len += line_part_len;
                line_buf[line_len] = '\0';
                
                if (line_len == 0) {
                    state = STATE_SEARCH;
                } else {
                    switch (state) {
                        case STATE_KEY:
                            if (ht.keys_size + line_len + sizeof(size_t) > ht.data_cap) {
                                double_buff(&ht);
                            }
                            *((size_t *)((ht.keys) + ht.keys_size)) = line_len;
                            ht.keys_size += sizeof(size_t);
                            ft_memcpy(&ht.keys[ht.keys_size], line_buf, line_len + 1);
                            last_key = &ht.keys[ht.keys_size];
                            ht.keys_size += ALIGN(line_len + 1);
                            state = STATE_VALUE;
                            break;
                            
                        case STATE_VALUE:
                            if (ht.values_size + (line_len) + sizeof(size_t) > ht.data_cap) {
                                double_buff(&ht);
                            }
                            *((size_t *)((ht.values) + ht.values_size)) = line_len;
                            ht.values_size += sizeof(size_t);
                            ft_memcpy(&ht.values[ht.values_size], line_buf, line_len + 1);
                            ht_insert(&ht, last_key, &ht.values[ht.values_size]);
                            ht.values_size += ALIGN(line_len + 1);
                            state = STATE_KEY;
                            break;
                            
                        case STATE_SEARCH:
                            {
                                void *result = ht_get(&ht, line_buf, line_len);
                                if (result) {
                                    write(1, (char *)result, *((size_t *)result - (size_t)1));
                                    write(1, "\n", 1);
                                } else {
                                    write(1, line_buf, strlen(line_buf));
                                    write(1, ": Not found\n", 12);
                                }
                            }
                            break;
                    }
                }
                buf_pos += line_part_len + 1;
                line_len = 0;
            } else {
                size_t remaining = bytes_read - buf_pos;
                ft_memcpy(line_buf + line_len, chunk + buf_pos, remaining);
                line_len += remaining;
                break;
            }
        }
    }

    if (line_len > 0 && state == STATE_SEARCH) {
        line_buf[line_len] = '\0';
        void *result = ht_get(&ht, line_buf, line_len);
        if (result) {
            write(1, (char *)result, *((size_t *)result - (size_t)1));
            write(1, "\n", 1);
        } else {
            write(1, line_buf, strlen(line_buf));
            write(1, ": Not found\n", 12);
        }
    }

    ht_free(&ht);    
    return 0;
}