/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   memchr.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lmicheli <lmicheli@student.42firenze.it>   +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/25 11:44:55 by lmicheli          #+#    #+#             */
/*   Updated: 2025/05/25 11:45:01 by lmicheli         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <hotrace.h>

#include <hotrace.h>

void *ft_memchr(const void *s, int c, size_t n)
{
    const unsigned char *src = s;
    unsigned char uc = c;
    
    // Handle small sizes with direct comparison
    if (n < 32) {
        while (n--) {
            if (*src == uc)
                return (void *)src;
            src++;
        }
        return NULL;
    }
    
	// Replace vpbroadcastb with SSE2 equivalent
	__asm__ volatile (
		"movd %0, %%xmm1\n"
		"punpcklbw %%xmm1, %%xmm1\n"
		"punpcklwd %%xmm1, %%xmm1\n"
		"pshufd $0, %%xmm1, %%xmm1\n"
		:
		: "r" ((uint32_t)uc)
		: "xmm1"
	);
    
    // Main loop - process 32 bytes at a time
    while (n >= 32) {
        uint32_t mask;
        __asm__ volatile (
            "vmovdqu (%1), %%ymm0\n"         // Load 32 bytes
            "vpcmpeqb %%ymm1, %%ymm0, %%ymm0\n" // Compare each byte
            "vpmovmskb %%ymm0, %0\n"         // Create mask of matches
            : "=r" (mask)
            : "r" (src)
            : "ymm0"
        );
        
        if (mask) {
            // Found a match - determine exact position
            return (void *)(src + __builtin_ctz(mask));
        }
        
        src += 32;
        n -= 32;
    }
    
    // Handle remaining bytes (0-31)
    while (n--) {
        if (*src == uc)
            return (void *)src;
        src++;
    }
    
    __asm__ volatile("vzeroupper" ::: "ymm0", "ymm1");
    return NULL;
}