#pragma once

#include <enc_types.h>

void rrprintf(int mode, char *fix_buf, int args_num, ...);

inline uint64_t get_timestamp(void)
{
    uint64_t tsc;
#if defined(__aarch64__) && 0 // this can be opened if kernel enables user space read of pmccntr reg.
    asm volatile("mrs %0, pmccntr_el0" : "=r"(tsc));
#elif defined(__x86_64__)
	unsigned int a, d;
	asm volatile("rdtscp" : "=a" (a), "=d" (d) : : "%rbx", "%rcx");
	tsc = ((unsigned long) a) | (((unsigned long) d) << 32);
#else
    tsc = 0;
#endif
    return tsc;
}
