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


#include "hotrace.h"

// --- MAIN ---

#include "hotrace.h"

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
    char *key = NULL;            // Current key being processed
    
    ssize_t bytes_read;
    ssize_t buf_pos = 0;          // Position in current chunk
    
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
                memcpy(line_buf + line_len, chunk + buf_pos, line_part_len);
                line_len += line_part_len;
                line_buf[line_len] = '\0';
                
                // Process the complete line based on current state
                if (line_len == 0) {
                    // Empty line - transition to search mode
                    state = STATE_SEARCH;
                } else {
                    switch (state) {
                        case STATE_KEY:
                            key = strdup(line_buf);
                            state = STATE_VALUE;
                            break;
                            
                        case STATE_VALUE:
                            ht_insert(&ht, key, strdup(line_buf));
                            free(key);
                            key = NULL;
                            state = STATE_KEY;
                            break;
                            
                        case STATE_SEARCH:
                            {
                                const char *result = ht_get(&ht, line_buf);
                                if (result) {
                                    size_t result_len = strlen(result);
                                    write(1, result, result_len);
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
    
    // Process any remaining line in buffer (should only happen for the last search key if it doesn't end with newline)
    if (line_len > 0 && state == STATE_SEARCH) {
        line_buf[line_len] = '\0';
        const char *result = ht_get(&ht, line_buf);
        if (result) {
            write(1, result, strlen(result));
            write(1, "\n", 1);
        } else {
            write(1, line_buf, strlen(line_buf));
            write(1, ": Not found\n", 12);
        }
    }
    
    // Clean up
    // Free all allocated memory in the hash table
    ht_free(&ht);
    
    return 0;
}
