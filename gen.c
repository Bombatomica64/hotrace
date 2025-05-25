#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <stdbool.h>

#define num_keys 200000
#define tests 1000
#define bad_test_chance 0

#define key_size 500
#define bad_key_size 250
#define data_size 25

int rand_int_exlusive(int v)
{
	return rand() % v;
}

bool chance(int chance)
{
	return rand_int_exlusive(100) < chance;
}

char rand_char()
{
	const static char* chars = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789_ ";
	const int num_chars = 64;

	return chars[rand_int_exlusive(num_chars)];
}

char* gen_key(int size)
{
	char* ptr = malloc(size * sizeof(char));

	for (int i = 0; i < size; i++)
		ptr[i] = rand_char();
	
	return ptr;
}

int main()
{
	char* keys[num_keys];

	for (int i = 0; i < num_keys; i++)
	{
		keys[i] = gen_key(key_size);

		write(STDOUT_FILENO, keys[i], key_size);
		write(STDOUT_FILENO, "\n", 1);
		char* data = gen_key(data_size);
		write(STDOUT_FILENO, data, data_size);
		write(STDOUT_FILENO, "\n", 1);
		free(data);
	}

	write(STDOUT_FILENO, "\n", 1);	// empty line

	for (int i = 0; i < tests; i++)
	{
		if (chance(bad_test_chance))
		{
			char* data = gen_key(bad_key_size);
			write(STDOUT_FILENO, data, bad_key_size);
			free(data);
		}
		else
		{
			char* key = keys[rand_int_exlusive(num_keys)];
			write(STDOUT_FILENO, key, key_size);
		}
		write(STDOUT_FILENO, "\n", 1);
	}
}