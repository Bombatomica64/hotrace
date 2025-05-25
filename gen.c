#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <stdbool.h>
#include <time.h>

// Reduced numbers to prevent memory issues
#define NUM_KEYS 1000000
#define TESTS 50000
#define BAD_TEST_CHANCE 1  // Percentage (0-100)

// Key length ranges
#define MIN_KEY_SIZE 20
#define MAX_KEY_SIZE 21

// Bad key length ranges
#define MIN_BAD_KEY_SIZE 20
#define MAX_BAD_KEY_SIZE 21

// Value length ranges
#define MIN_VALUE_SIZE 20
#define MAX_VALUE_SIZE 21

int rand_int_range(int min, int max)
{
    return min + rand() % (max - min + 1);
}

int rand_int_exclusive(int v)
{
    return rand() % v;
}

bool chance(int percentage)
{
    return rand_int_exclusive(100) < percentage;
}

char rand_char()
{
    const static char* chars = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789_ ";
    const int num_chars = 64;

    return chars[rand_int_exclusive(num_chars)];
}

char* gen_key(int size)
{
    char* ptr = malloc(size * sizeof(char));

    for (int i = 0; i < size; i++)
        ptr[i] = rand_char();
    
    return ptr;
}

typedef struct {
    char* data;
    int length;
} string_t;

int main()
{
    srand(time(NULL));  // Initialize random seed
    
    // Allocate on heap instead of stack
    string_t* keys = malloc(NUM_KEYS * sizeof(string_t));
    if (!keys) {
        fprintf(stderr, "Memory allocation failed\n");
        return 1;
    }

    for (int i = 0; i < NUM_KEYS; i++)
    {
        int key_length = rand_int_range(MIN_KEY_SIZE, MAX_KEY_SIZE);
        keys[i].data = gen_key(key_length);
        keys[i].length = key_length;

        // Write key
        write(STDOUT_FILENO, keys[i].data, key_length);
        write(STDOUT_FILENO, "\n", 1);
        
        // Generate and write value
        int value_length = rand_int_range(MIN_VALUE_SIZE, MAX_VALUE_SIZE);
        char* value = gen_key(value_length);
        write(STDOUT_FILENO, value, value_length);
        write(STDOUT_FILENO, "\n", 1);
        free(value);
    }

    write(STDOUT_FILENO, "\n", 1);  // Empty line to mark end of key-value pairs

    // Generate test queries
    for (int i = 0; i < TESTS; i++)
    {
        if (chance(BAD_TEST_CHANCE))
        {
            // Generate a "bad" key (likely not in the database)
            int bad_key_length = rand_int_range(MIN_BAD_KEY_SIZE, MAX_BAD_KEY_SIZE);
            char* bad_key = gen_key(bad_key_length);
            write(STDOUT_FILENO, bad_key, bad_key_length);
            free(bad_key);
        }
        else
        {
            // Use a key from our database
            int idx = rand_int_exclusive(NUM_KEYS);
            write(STDOUT_FILENO, keys[idx].data, keys[idx].length);
        }
        write(STDOUT_FILENO, "\n", 1);
    }

    // Free allocated memory
    for (int i = 0; i < NUM_KEYS; i++)
    {
        free(keys[i].data);
    }
    free(keys);  // Don't forget to free the keys array itself
    
    return 0;
}