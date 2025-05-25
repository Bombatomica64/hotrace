/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   strcmp.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lmicheli <lmicheli@student.42firenze.it>   +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/25 11:28:57 by lmicheli          #+#    #+#             */
/*   Updated: 2025/05/25 11:29:00 by lmicheli         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <hotrace.h>

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
