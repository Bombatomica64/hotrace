/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lmicheli <lmicheli@student.42firenze.it>   +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/24 09:57:55 by lmicheli          #+#    #+#             */
/*   Updated: 2025/05/24 09:59:57 by lmicheli         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */


#include <hotrace.h>

// --- MAIN ---

#include <hotrace.h>

#include <time.h>
#include <stdio.h>

#define CHUNK_SIZE (1 << 20)  // 1MB chunks
// typedef enum {
//     STATE_KEY,       // Reading a key
//     STATE_VALUE,     // Reading a value
//     STATE_SEARCH,    // Reading search keys
// } parse_state_t;


int main(void)
{


struct timespec start, end;
    clock_gettime(CLOCK_MONOTONIC, &start);

    char chunk[CHUNK_SIZE + 1];  // +1 for null termination
    char line_buf[CHUNK_SIZE];   // Buffer for partial lines
    size_t line_len = 0;         // Current line buffer length
    // Initial setup
    t_ht ht = ht_create(1024);   // Start with reasonable size, will resize
    parse_state_t state = STATE_KEY;
    
    char **value_list = malloc(sizeof(char *) * 10);
    int value_index = 0;
    size_t value_offset = 0;
    value_list[value_index] = malloc(4096 * 4096);
    
    char **key_list = malloc(sizeof(char *) * 10);
    int key_index = 0;
    size_t key_offset = 0;
    key_list[key_index] = malloc(4096 * 4096);

    ssize_t bytes_read;
    ssize_t buf_pos = 0;
    char *last_key;
    while ((bytes_read = read(0, chunk, CHUNK_SIZE)) > 0) {
        chunk[bytes_read] = '\0';
        buf_pos = 0;
        // Process each character in the chunk
        while (buf_pos < bytes_read) {
            // Find the next newline or end of chunk
            char *newline = ft_memchr(chunk + buf_pos, '\n', bytes_read - buf_pos);
            
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
                            if (key_offset + line_len > 4096 * 4096) {
                                key_index++;
                                key_list[key_index] = malloc(4096 * 4096);
                                key_offset = 0;
                            }
                            *((size_t *)((key_list[key_index]) + key_offset)) = line_len;
                            key_offset += sizeof(size_t);
                            ft_memcpy(&key_list[key_index][key_offset], line_buf, line_len + 1);
                            last_key = &key_list[key_index][key_offset];
                            key_offset += line_len + 1;
                            state = STATE_VALUE;
                            break;
                            
                        case STATE_VALUE:
                            if (value_offset + line_len > 4096 * 4096) {
                                value_index++;
                                value_list[value_index] = malloc(4096 * 4096);
                                value_offset = 0;
                            }
                            *((size_t *)((value_list[value_index]) + value_offset)) = line_len;
                            value_offset += sizeof(size_t);
                            ft_memcpy(&value_list[value_index][value_offset], line_buf, line_len + 1);
                            ht_insert(&ht, last_key, &value_list[value_index][value_offset]);
                            value_offset += line_len + 1;
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
                memcpy(line_buf + line_len, chunk + buf_pos, remaining);
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
    free(key_list);
    free(value_list);
	clock_gettime(CLOCK_MONOTONIC, &end);
	double time_taken = (end.tv_sec - start.tv_sec) + 
	(end.tv_nsec - start.tv_nsec) / 1e9;
fprintf(stderr, "Execution time: %.9f seconds\n", time_taken);
return 0;
}