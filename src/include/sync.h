#pragma once

#ifdef __x86_64
#define YIELD_PROCESSOR __asm__ volatile("pause")
#elif __aarch64__
#define YIELD_PROCESSOR __asm__ volatile("yield")
#endif

#ifdef __cplusplus
extern "C" {
#endif

void spin_lock(int volatile* p);
void spin_unlock(int volatile* p);
void spin_wait(int volatile* p, int val);

#ifdef __cplusplus
}
#endif