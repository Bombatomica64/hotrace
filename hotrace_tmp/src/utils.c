#include "../include/utils.h"
#include "../include/hotrace.h"

void *ft_align_memcpy(void *dst, const void *src, size_t len)
{
	size_t i;
	unsigned long int *longword_ptr;
	unsigned long int longword;

	i = 0;
	if ((uintptr_t)dst % sizeof(long) == 0 && (uintptr_t)src % sizeof(long) == 0 \
		&& len % sizeof(long) == 0)
	{
		longword_ptr = (unsigned long int *)dst;
		const unsigned long int *s = (unsigned long int *)src;
		while (i < len / sizeof(unsigned long int))
		{
			longword_ptr[i] = s[i];
			longword = longword_ptr[i];
			if (((longword - LOMAGIC) & ~longword & HIMAGIC) != 0)
			{
				return dst;
			}
			i++;
		}
	}
	else
		ft_memcpy(dst, src, len);
	return dst;
}

void *ft_memcpy(void *dst, const void *src, size_t len)
{
	size_t i;
	char *d = dst;
	const char *s = src;

	i = 0;
	while (i < len)
	{
		d[i] = s[i];
		if (d[i] == 0)
			return dst;
		i++;
	}
		
	return dst;
}

void double_buff(t_ht *ht)
{
	ht->data_cap *= 2;
	char *new_keys = malloc(ht->data_cap);
	char *new_values = malloc(ht->data_cap);
	size_t  keys_offset = 0;
	size_t  values_offset = 0;

	for (size_t i = 0; i < ht->cap; i++) {
		if (!ht->tbl[i].k) {
			continue;
		}
		size_t s = (*((size_t *)ht->tbl[i].k - 1));
		*((size_t *)((new_keys) + keys_offset)) = s;
		keys_offset += sizeof(size_t);
		ft_align_memcpy(
			&new_keys[keys_offset],
			ht->tbl[i].k,
			s + 1
		);
		ht->tbl[i].k = &new_keys[keys_offset];
		keys_offset += ALIGN(s + 1);

		size_t s2 = (*((size_t *)ht->tbl[i].v - 1));
		*((size_t *)((new_values) + values_offset)) = s2;
		values_offset += sizeof(size_t);
		ft_align_memcpy(
			&new_values[values_offset],
			ht->tbl[i].v,
			s2 + 1
		);
		ht->tbl[i].v = &new_values[values_offset];
		values_offset += ALIGN(s2 + 1);
	}
	free(ht->keys);
	free(ht->values);
	ht->keys = new_keys;
	ht->keys_size = keys_offset;
	ht->values = new_values;
	ht->values_size = values_offset;
}

int	ft_strcmp(const char *s1, const char *s2)
{
	int	result;

	__asm__ volatile (
		// Save registers we'll modify
		"push %%rcx\n"
		// Main comparison loop
		"1:\n"
		"movzbl (%0), %%eax\n" // Load byte from s1 into eax
		"movzbl (%1), %%ecx\n" // Load byte from s2 into ecx
		"incq %0\n"// Increment s1 pointer
		"incq %1\n"// Increment s2 pointer
		"testb %%al, %%al\n"// Check if we reached end of s1
		"je 2f\n"// If zero, exit loop
		"cmpb %%cl, %%al\n"// Compare bytes
		"je 1b\n"// If equal, continue loop
		// Exit point - compute difference
		"2:\n"
		"subl %%ecx, %%eax\n"// Compute difference
		"pop %%rcx\n"// Restore rcx
		: "=&S"(s1), "=&D"(s2), "=&a"(result)
		: "0"(s1), "1"(s2)
		: "memory", "cc");
	return (result);
}

static void	*ft_memchr_small(const unsigned char *src, unsigned char uc,
	size_t n)
{
while (n--)
{
	if (*src == uc)
		return ((void *)src);
	src++;
}
return (NULL);
}

static void	ft_broadcast_char(unsigned char uc)
{
__asm__ volatile("movd %0, %%xmm1\n"
					"punpcklbw %%xmm1, %%xmm1\n"
					"punpcklwd %%xmm1, %%xmm1\n"
					"pshufd $0, %%xmm1, %%xmm1\n"
					:
					: "r"((uint32_t)uc)
					: "xmm1");
}

static void	*ft_memchr_avx2(const unsigned char *src, size_t *n)
{
	uint32_t mask;

while (*n >= 32)
{
	__asm__ volatile("vmovdqu (%1), %%ymm0\n"
						"vpcmpeqb %%ymm1, %%ymm0, %%ymm0\n"
						"vpmovmskb %%ymm0, %0\n"
						: "=r"(mask)
						: "r"(src)
						: "ymm0");
	if (mask)
		return (void *)(src + __builtin_ctz(mask));
	src += 32;
	*n -= 32;
}
return ((void *)src);
}

void	*ft_memchr(const void *s, int c, size_t n)
{
const unsigned char *src = s;
unsigned char uc = c;
const unsigned char *end_ptr;

if (n < 32)
	return (ft_memchr_small(src, uc, n));

ft_broadcast_char(uc);

end_ptr = ft_memchr_avx2(src, &n);
if (end_ptr != src)
	src = end_ptr;

// Handle remaining bytes
void *result = ft_memchr_small(src, uc, n);

__asm__ volatile("vzeroupper" ::: "ymm0", "ymm1");
return (result);
}