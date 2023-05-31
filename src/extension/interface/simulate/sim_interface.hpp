#pragma once 

#ifdef __cplusplus
extern "C" {
#endif

void *getSharedBuffer(size_t size);
void freeBuffer(void *buffer);

void *getMockBuffer(size_t size);

#ifdef __cplusplus
}
#endif
