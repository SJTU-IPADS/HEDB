// #include <thread>
#include "extension.hpp"
#include "ops_server.h"
#include "request_types.h"
#include <cassert>
#include <cstring>
#include <defs.h>
#include <errno.h>
#include <fcntl.h>
#include <sim_interface.hpp>
#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <sys/shm.h>
#include <sys/types.h>
#include <unistd.h>

#include <chrono>
using namespace std::chrono;

/* --------------------------------------------- */
/* this load barrier is only for arm */
#ifdef __aarch64__
#define LOAD_BARRIER asm volatile("dsb ld" :: \
                                      : "memory")
#define STORE_BARRIER asm volatile("dsb st" :: \
                                       : "memory")
#elif __x86_64
#define LOAD_BARRIER ;
#define STORE_BARRIER ;
#endif

void forkOpsProcess()
{
    int keyid = getpid(); // keyid is parrent pid
    pid_t pid = fork();
    if (pid == 0) { // child
        char keyid_str[64];
        sprintf(keyid_str, "%d", keyid);

        // char ch[100];
        // sprintf(ch, "keyid_str: %s", keyid_str);
        // print_info(ch);
        char arg0[] = "sim_ops", env0[] = "";
        char* argv[] = { arg0, keyid_str, NULL };
        char* envp[] = { env0, NULL };
        execve("/usr/local/lib/hedb/sim_ops", argv, envp);
    }
}

static int posix_shmid;
static void* posix_shm_addr = NULL;
// static void posix_shm_exit_handler(){

// }

static int shm_key = 666; /* shmkey 666 is for real udf, shmkey 233 is for mock */
static void* get_shmem_posix(size_t size)
{
    // 1. create shared memory
    // int key = 666;
    posix_shmid = shmget(shm_key, size, 0666 | IPC_CREAT);
    if (posix_shmid == -1) {
        perror("shmget failed %d errno\n");
        exit(EXIT_FAILURE);
    }
    // 2. attach shared memory
    posix_shm_addr = shmat(posix_shmid, NULL, 0);
    if (posix_shm_addr == (void*)-1) {
        perror("shmat failed\n");
        exit(EXIT_FAILURE);
    }
    // atexit(posix_shm_exit_handler);
    return posix_shm_addr;
}

static void* get_shmem_ivshm(size_t size)
{
    int fd = open("/dev/uio0", O_RDWR);
    assert(fd != -1);

    void* p = mmap(0, size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    assert(p != NULL);
    return p;
}
#define SHM_SIZE (16 * 1024 * 1024)
#define META_REQ_SIZE 1024
#define REQ_REGION_SIZE 1024 * 1024
#define REGION_N_OFFSET(n) (META_REQ_SIZE + REQ_REGION_SIZE * n)

void* getMockBuffer(size_t size)
{
    shm_key = 233; // 233 is the mock key
    return getSharedBuffer(size);
}

static void* shm_addr;
static int shm_id;
void* getSharedBuffer(size_t size)
{
#ifdef ENABLE_LOCAL_SIM
    shm_addr = get_shmem_posix(SHM_SIZE);
#else
    shm_addr = get_shmem_ivshm(SHM_SIZE);
#endif
    OpsServer* ops_server = (OpsServer*)shm_addr;
    // print_info("get shmem\n");

    spin_lock(&ops_server->lock);
    assert(ops_server->status == SHM_NONE);

    // print_info("lock got\n");
    auto start = std::chrono::high_resolution_clock::now();

    ops_server->status = SHM_GET;
    while (ops_server->status != SHM_DONE) {
        YIELD_PROCESSOR;
        auto elapsed = std::chrono::high_resolution_clock::now() - start;
        long long microseconds = std::chrono::duration_cast<std::chrono::microseconds>(
            elapsed)
                                     .count();
        if (microseconds > 5000) {
            /* unable to connect to udf */
            return 0;
        }
    }
    LOAD_BARRIER;
    shm_id = ops_server->ret_id;
    void* buffer = shm_addr + REGION_N_OFFSET(shm_id);

    // char ch[100];
    // sprintf(ch, "server done %d. shm_addr %p, buffer %p\n", ops_server->ret_id, shm_addr, buffer);
    // print_info(ch);

    STORE_BARRIER;
    ops_server->status = SHM_NONE;
    spin_unlock(&ops_server->lock);

    return buffer;
}
void freeBuffer(void* buffer)
{
    OpsServer* ops_server = (OpsServer*)shm_addr;
    spin_lock(&ops_server->lock);
    assert(ops_server->status == SHM_NONE);

    ops_server->free_id = shm_id; // free shm_id buffer
    STORE_BARRIER;
    ops_server->status = SHM_FREE;
    while (ops_server->status != SHM_DONE) {
        YIELD_PROCESSOR;
    }
    ops_server->status = SHM_NONE;
    spin_unlock(&ops_server->lock);
}