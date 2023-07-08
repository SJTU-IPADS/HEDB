#include <sync.h>

void spin_lock(int volatile* p)
{
    while (!__sync_bool_compare_and_swap(p, 0, 1)) {
        while (*p)
#ifdef __x86_64
            __asm__("pause");
#else /* ARM */
            __asm__ __volatile__("yield" ::: "memory");
#endif
        ;
    }
}

void spin_unlock(int volatile* p)
{
    asm volatile(""); // acts as a memory barrier.
    *p = 0;
}

void spin_wait(int volatile* p, int val)
{
    while (*p != val)
#ifdef __x86_64
        __asm__ volatile("pause" ::: "memory");
#else /* ARM */
        __asm__ __volatile__("yield" ::: "memory");
#endif
}
