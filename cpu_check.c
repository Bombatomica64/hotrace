#include <cpuid.h>
#include <hotrace.h>

// CPU feature flags
static int	has_sse2 = 0;
static int	has_avx2 = 0;
static int	has_avx512 = 0;
static int	cpu_features_detected = 0;

void	detect_cpu_features(void)
{
	unsigned int eax, ebx, ecx, edx;
	if (cpu_features_detected)
		return ;
	if (__get_cpuid(1, &eax, &ebx, &ecx, &edx))
	{
		has_sse2 = (edx & bit_SSE2) != 0;
	}
	if (__get_cpuid_count(7, 0, &eax, &ebx, &ecx, &edx))
	{
		has_avx2 = (ebx & bit_AVX2) != 0;
	}
	cpu_features_detected = 1;
}

int	cpu_has_sse2(void)
{
	return (has_sse2);
}
int	cpu_has_avx2(void)
{
	return (has_avx2);
}
int	cpu_has_avx512(void)
{
	return (has_avx512);
}