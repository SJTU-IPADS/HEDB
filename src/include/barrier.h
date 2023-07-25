#pragma once

/* this load barrier is only for arm */
#ifdef __x86_64__
#define LOAD_BARRIER
#define STORE_BARRIER
#elif __aarch64__
#define LOAD_BARRIER asm volatile("dsb ld" ::: "memory")
#define STORE_BARRIER asm volatile("dsb st" ::: "memory")
#endif
