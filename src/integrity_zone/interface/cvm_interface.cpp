// SPDX-License-Identifier: Mulan PSL v2
/*
 * Copyright (c) 2021 - 2023 The HEDB Project.
 */

#include "cvm_interface.hpp"
#include "barrier.h"
#include "extension.hpp"
#include "ops_server.h"
#include "request_types.h"

#include <cassert>
#include <cstring>
#include <defs.h>
#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <sys/shm.h>
#include <sys/types.h>
#include <unistd.h>

#include <chrono>
using namespace std::chrono;

static void* get_shmem_ivshm(size_t size)
{
    int fd;
    if ((fd = open("/dev/shm/ivshmem", O_RDWR)) == -1)
        fd = open("/dev/uio0", O_RDWR);
    assert(fd != -1);

    void* p = mmap(0, size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    assert(p != NULL);
    return p;
}

#define SHM_SIZE (16 * 1024 * 1024)
#define META_REQ_SIZE 1024
#define REQ_REGION_SIZE 1024 * 1024
#define REGION_N_OFFSET(n) (META_REQ_SIZE + REQ_REGION_SIZE * n)

static void* shm_addr;
static int shm_id;
void* getSharedBuffer(size_t size)
{
    shm_addr = get_shmem_ivshm(SHM_SIZE);
    OpsServer* ops_server = (OpsServer*)shm_addr;
    spin_lock(&ops_server->lock);
    assert(ops_server->status == SHM_NONE);

    auto start = std::chrono::high_resolution_clock::now();

    ops_server->status = SHM_GET;
    while (ops_server->status != SHM_DONE) {
        YIELD_PROCESSOR;
        auto elapsed = std::chrono::high_resolution_clock::now() - start;
        long long microseconds = std::chrono::duration_cast<std::chrono::microseconds>(elapsed).count();
        if (microseconds > 5000) {
            /* unable to connect to ops */
            return 0;
        }
    }
    LOAD_BARRIER;
    shm_id = ops_server->ret_id;
    void* buffer = (char*)shm_addr + REGION_N_OFFSET(shm_id);

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
