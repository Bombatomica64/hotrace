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

#define CHUNK_SIZE (1 << 20)  // 1MB chunks

typedef enum {
    STATE_KEY,       // Reading a key
    STATE_VALUE,     // Reading a value
    STATE_SEARCH,    // Reading search keys
} parse_state_t;

int main(void)
{
    char chunk[CHUNK_SIZE + 1];  // +1 for null termination
    char line_buf[CHUNK_SIZE];   // Buffer for partial lines
    size_t line_len = 0;         // Current line buffer length
    // Initial setup
    t_ht ht = ht_create(1024);   // Start with reasonable size, will resize
    parse_state_t state = STATE_KEY;
    
    char **value_list = malloc(sizeof(char *) * 10);
    int value_index = 0;
    size_t value_size = 0;
    value_list[value_index] = malloc(4096 * 4096);
    
    char **key_list = malloc(sizeof(char *) * 10);
    int key_index = 0;
    size_t key_size = 0;
    key_list[key_index] = malloc(4096 * 4096);

    ssize_t bytes_read;
    ssize_t buf_pos = 0;
    size_t key_old_size = 0;
    char *last_key;
    while ((bytes_read = read(0, chunk, CHUNK_SIZE)) > 0) {
        chunk[bytes_read] = '\0';
        buf_pos = 0;
        // Process each character in the chunk
        while (buf_pos < bytes_read) {
            // Find the next newline or end of chunk
            char *newline = memchr(chunk + buf_pos, '\n', bytes_read - buf_pos);
            
            if (newline) {
                // Complete line found
                size_t line_part_len = newline - (chunk + buf_pos);
                
                // Copy this part to line buffer
                ft_memcpy(line_buf + line_len, chunk + buf_pos, line_part_len);
                line_len += line_part_len;
                line_buf[line_len] = '\0';
                
                // Process the complete line based on current state
                if (line_len == 0) {
                    // Empty line - transition to search mode
                    state = STATE_SEARCH;
                } else {
                    switch (state) {
                        case STATE_KEY:
                            if (key_size + line_len > 4096 * 4096) {
                                key_index++;
                                key_list[key_index] = malloc(4096 * 4096);
                                key_size = 0;
                            }
                            ft_memcpy(&key_list[key_index][key_size], line_buf, line_len + 1);
                            last_key = &key_list[key_index][key_size];
                            key_old_size = line_len;
                            key_size += line_len + 1;
                            // printf("line char: %c\n", key_list[key_index][key_size]);
                            key_list[key_index][key_size] = 0;
                            state = STATE_VALUE;
                            break;
                            
                        case STATE_VALUE:
                            if (value_size + line_len > 4096 * 4096) {
                                value_index++;
                                value_list[value_index] = malloc(4096 * 4096);
                                value_size = 0;
                            }
                            ft_memcpy(&value_list[value_index][value_size], line_buf, line_len + 1);
                            ht_insert(&ht, last_key, key_old_size, &value_list[value_index][value_size], line_len);
                            value_size += line_len + 1;
                            state = STATE_KEY;
                            break;
                            
                        case STATE_SEARCH:
                            {
                                const sstring result = ht_get(&ht, line_buf, line_len);
                                if (result.str) {
                                    write(1, result.str, result.size);
                                    write(1, "\n", 1);
                                } else {
                                    write(1, line_buf, strlen(line_buf));
                                    write(1, ": Not found\n", 12);
                                }
                            }
                            break;
                    }
                }
                
                // Move past the newline
                buf_pos += line_part_len + 1;
                line_len = 0;
            } else {
                // Partial line, store in buffer and continue with next chunk
                size_t remaining = bytes_read - buf_pos;
                memcpy(line_buf + line_len, chunk + buf_pos, remaining);
                line_len += remaining;
                break;
            }
        }
    }

    if (line_len > 0 && state == STATE_SEARCH) {
        line_buf[line_len] = '\0';
        sstring result = ht_get(&ht, line_buf, line_len);
        if (result.str) {
            write(1, result.str, result.size);
            write(1, "\n", 1);
        } else {
            write(1, line_buf, strlen(line_buf));
            write(1, ": Not found\n", 12);
        }
    }

    ht_free(&ht);    
    return 0;
}