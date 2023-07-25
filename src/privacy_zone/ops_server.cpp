#include <fcntl.h>
#include <mutex>
#include <pthread.h>
#include <sched.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/shm.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

#include "barrier.h"
#include "crypto.h"
#include "debug.h"
#include "enc_ops.h"
#include "ops_server.h"
#include "request_types.h"
#include "sync.h"
#include <fcntl.h>
#include <mutex>
#include <sched.h>
#include <sys/mman.h>
#include <sys/wait.h>

#include "ops_server.hpp"

#include <iostream>
#include <map> // for profile

struct alignas(128) Decrypt_args {
    bool inited;
    int index;
    volatile int decrypt_status;
    uint8_t* decrypt_dst;
    uint8_t* decrypt_src;
    size_t decrypt_src_len;
    size_t decrypt_dst_len;
};

static struct Decrypt_args args_array[MAX_DECRYPT_THREAD];

uint8_t decrypt_buffer[sizeof(EncCStr) * 2];
uint8_t plain_buffer[2048];
void* decrypt_thread(void* arg)
{
    int index = *(int*)arg;

    while (1) {
        if (args_array[index].decrypt_status == SENT) {
            LOAD_BARRIER;
            decrypt_bytes(args_array[index].decrypt_src, args_array[index].decrypt_src_len,
                args_array[index].decrypt_dst, args_array[index].decrypt_dst_len);
            STORE_BARRIER;
            args_array[index].decrypt_status = DONE;
        } else if (args_array[index].decrypt_status == EXIT) {
            break;
        } else {
            ;
        }
    }
    return 0;
}

// static unsigned long decrypt_para_counter = 0;
int decrypt_bytes_para(uint8_t* pSrc, size_t src_len, uint8_t* pDst, size_t exp_dst_len)
{
#ifdef ENABLE_PARA
    // decrypt_para_counter++;

    int i;
find_thread:
    for (i = 0; i < MAX_DECRYPT_THREAD; i++) {
        if (args_array[i].inited && args_array[i].decrypt_status == DONE) {
            LOAD_BARRIER;
            break;
        } else if (!args_array[i].inited) {
            pthread_t thread;
            pthread_create(&thread, nullptr, decrypt_thread, &(args_array[i].index));
            args_array[i].inited = true;
            break;
        } else {
            ;
        }
    }

    if (i == MAX_DECRYPT_THREAD) {
        printf("try another round\n");
        goto find_thread;
    }

    args_array[i].decrypt_src = pSrc;
    args_array[i].decrypt_src_len = src_len;
    args_array[i].decrypt_dst = pDst;
    args_array[i].decrypt_dst_len = exp_dst_len;
    STORE_BARRIER;
    args_array[i].decrypt_status = SENT;

#else
    decrypt_bytes(pSrc, src_len, pDst, exp_dst_len);
#endif
    return 0;
}

void decrypt_wait(uint8_t* pDst, size_t exp_dst_len)
{
#ifdef ENABLE_PARA
    bool done = false;
    while (!done) {
        done = true;
        for (int i = 0; i < MAX_DECRYPT_THREAD; i++) {
            if (args_array[i].inited && args_array[i].decrypt_status != DONE) {
                done = false;
                break;
            }
        }
        YIELD_PROCESSOR;
    }
    LOAD_BARRIER;
#endif
    // memcpy(pDst, plain_buffer, exp_dst_len);
}

static unsigned long encrypt_counter = 0;
int encrypt_bytes(uint8_t* pSrc, size_t src_len, uint8_t* pDst, size_t exp_dst_len)
{
    size_t dst_len = exp_dst_len;
    int resp = 0;

    resp = gcm_encrypt(pSrc, src_len, pDst, &dst_len);
    assert(dst_len == exp_dst_len);

    encrypt_counter++;
    return resp;
}

static unsigned long decrypt_counter = 0;
int decrypt_bytes(uint8_t* pSrc, size_t src_len, uint8_t* pDst, size_t exp_dst_len)
{
    size_t dst_len = 0;
    int resp = 0;

    if (src_len <= IV_SIZE + TAG_SIZE) {
        printf("error src len");
        return -1;
    }
    resp = gcm_decrypt(pSrc, src_len, pDst, &dst_len);
    if (resp != 0) {
        _print_hex("dec from ", pSrc, src_len);
        _print_hex("dec to ", pDst, dst_len);
    }

    decrypt_counter++;
    return resp;
}

FILE* write_ptr = 0;
// #define printf(...) fprintf(write_ptr, __VA_ARGS__ )

#define SHM_SIZE (16 * 1024 * 1024)
#define META_REQ_SIZE 1024
#define REQ_REGION_SIZE 1024 * 1024
#define REGION_N_OFFSET(n) (META_REQ_SIZE + REQ_REGION_SIZE * n)
#define MAX_REGION_NUM 16

int ivshm_fd;
void ivshm_exit_handler()
{
    close(ivshm_fd);
}
void* get_shmem_ivshm(size_t size)
{
    if ((ivshm_fd = open("/dev/shm/ivshmem", O_RDWR)) == -1) {
        ivshm_fd = open("/dev/uio0", O_RDWR);
    } else {
        fprintf(stderr, "[INFO] cannot find /dev/uio0, using local /dev/shm instead\n");
    }
    assert(ivshm_fd != -1);

    void* ivshm_p = mmap(0, size, PROT_READ | PROT_WRITE, MAP_SHARED, ivshm_fd, 0);
    assert(ivshm_p != NULL);

    atexit(ivshm_exit_handler);
    return ivshm_p;
}

pid_t fork_ops_process(void* shm_addr)
{
    pid_t pid = fork();

    // father
    if (pid != 0) {
        return pid;
    }

    // child
    // after fork, child inherit all attached shared memory (man shmat)
    pid = getpid();
    // printf("[%d] waiting on shm_addr %p\n", pid, shm_addr);

    for (int i = 0; i < MAX_DECRYPT_THREAD; i++) {
        args_array[i].index = i;
        args_array[i].inited = false;
        args_array[i].decrypt_status = NONE;
    }

    int counter = 0, non_enc_counter = 0;

    BaseRequest* req = (BaseRequest*)shm_addr;
    while (1) {
        if (req->status == EXIT) {
            // decrypt_status = EXIT;
            for (int i = 0; i < MAX_DECRYPT_THREAD; i++) {
                if (args_array[i].inited) {
                    args_array[i].decrypt_status = EXIT;
                }
            }
            printf("[%d] total ops: %d, enc: %ld, dec: %ld, others: %d\n", pid,
                counter, encrypt_counter, decrypt_counter, non_enc_counter);
            req->status = NONE;
            exit(0);
        } else if (req->status == SENT) {
            LOAD_BARRIER;
            // auto start = std::chrono::system_clock::now();
            // printf("request received %d\n", req->reqType);
            counter++;
            if (req->reqType != CMD_INT_ENC && req->reqType != CMD_INT_DEC && req->reqType != CMD_FLOAT_ENC && req->reqType != CMD_FLOAT_DEC && req->reqType != CMD_STRING_ENC && req->reqType != CMD_STRING_DEC && req->reqType != CMD_TIMESTAMP_ENC && req->reqType != CMD_TIMESTAMP_DEC)
                non_enc_counter++;
            if (req->reqType > 0)
                handle_ops(req);
            else
                ;
            // desenitize_ops(req);

            if (req->resp != 0) {
                printf("TA error %d, %d\n", req->resp, counter);
            }
            STORE_BARRIER;
            req->status = DONE;
        } else {
            YIELD_PROCESSOR;
        }
    }

    // child should never return.
    exit(0);
}

int main(int argc, char* argv[])
{
    int data_size = SHM_SIZE;
    pid_t child_pids[20] = {};
    OpsServer* req = (OpsServer*)get_shmem_ivshm(data_size);
    printf("HEDB ops_server running on shared memory addr: %p.\n", req);
    memset(req, 0, sizeof(OpsServer));
    while (1) {
        if (req->status == SHM_GET) {
            for (int i = 0; i < MAX_REGION_NUM; i++) {
                if (GET(req->bitmap, i) == 0) {
                    SET(req->bitmap, i);
                    req->ret_id = i;
                    void* addr = (char*)req + REGION_N_OFFSET(i);
                    pid_t child = fork_ops_process(addr);
                    child_pids[i] = child;
                    break;
                }
            }

            STORE_BARRIER; // store everything before DONE.
            req->status = SHM_DONE;
        } else if (req->status == SHM_FREE) {
            LOAD_BARRIER;
            int id = req->free_id;
            BaseRequest* base_req;
            base_req = (BaseRequest*)((char*)req + REGION_N_OFFSET(id));

            base_req->status = EXIT;
            assert(GET(req->bitmap, id) == 1);
            CLEAR(req->bitmap, id);
            waitpid(child_pids[id], nullptr, 0); // wait until child pid exit;

            STORE_BARRIER; // store everything before DONE.
            req->status = SHM_DONE;
        } else {
            YIELD_PROCESSOR;
        }
    }
}
