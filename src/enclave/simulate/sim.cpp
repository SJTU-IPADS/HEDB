#include <fcntl.h>
#include <mutex>
#include <pthread.h>
#include <sched.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/shm.h>
#include <sys/wait.h>
#include <unistd.h>

#include "crypto.h"
#include "enc_ops.h"
#include "ops_server.h"
#include "request_types.h"
#include "sim.hpp"
#include "sync.h"
#include <fcntl.h>
#include <mutex>
#include <sched.h>
#include <sys/mman.h>
#include <sys/wait.h>

// #include <chrono>
// #include <ctime>
// #include <sched.h>
#include <iostream>
#include <map> // for profile

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

static unsigned long decrypt_counter = 0;
int decrypt_bytes(uint8_t* pSrc, size_t src_len, uint8_t* pDst, size_t exp_dst_len)
{
    // _print_hex("dec ", pSrc, src_len);
    decrypt_counter++;

    size_t dst_len = 0;
    int resp = 0;

    // uint64_t duration =0,timer = 0;
    // uint64_t start,end;
    // uint32_t freq = read_cntfrq();
    // start = read_cntpct();

    // for (size_t i = 0; i < 1000; i++)
    // {
    // uint8_t *iv_pos = pSrc;
    // uint8_t *tag_pos = pSrc+IV_SIZE;
    // uint8_t *data_pos = pSrc+IV_SIZE+TAG_SIZE;
    // _print_hex("dec--enc-iv: ", (void *)iv_pos, IV_SIZE);
    // _print_hex("dec--enc-tag: ", (void *)tag_pos, TAG_SIZE);
    // _print_hex("dec--enc-txt: ", (void *)data_pos, exp_dst_len);
    if (src_len <= IV_SIZE + TAG_SIZE) {
        printf("error src len");
        return -1;
    }
    resp = gcm_decrypt(pSrc, src_len, pDst, &dst_len);
    // memcpy(pDst, pSrc, exp_dst_len);
    // printf("after dec");
    // assert(dst_len == exp_dst_len);

    // }

    // end= read_cntpct();
    // duration = (end - start) * 1000000 / freq;
    // timer += duration;
    // printf("1000 dec duration in us %d",duration);

    if (resp != 0) {
        // static __thread pid_t tid = gettid();
        // printf("tid %d, resp %d\n", tid,  resp);
        _print_hex("dec from ", pSrc, src_len);
        _print_hex("dec to ", pDst, dst_len);
    }

    return resp;
}

/* Encrypts byte array by aesgcm mode
 @input: uint8_t array - pointer to a byte array
         size_t - length of the array
         uint8_t array - pointer to result array
         size_t - length of result array (IV_SIZE + length of array +
 TAG_SIZE)
 @return:
    * SGX_error, if there was an error during encryption/decryption
    0, otherwise
*/
static unsigned long encrypt_counter = 0;
int encrypt_bytes(uint8_t* pSrc, size_t src_len, uint8_t* pDst, size_t exp_dst_len)
{
    encrypt_counter++;
    size_t dst_len = exp_dst_len;
    int resp = 0;

    resp = gcm_encrypt(pSrc, src_len, pDst, &dst_len);
    // printf("after enc");
    assert(dst_len == exp_dst_len);
    // {
    // 	_print_hex("enc from ", pSrc, src_len);
    // 	_print_hex("enc to ", pDst, dst_len);
    // }
    // memset(pDst, 0, dst_len);
    // memcpy(pDst, pSrc, dst_len);

    return resp;
}

FILE* write_ptr = 0;
// #define printf(...) fprintf(write_ptr, __VA_ARGS__ )

int shmid;
void* posix_shm_addr = NULL;
void posix_shm_exit_handler()
{
    // 4. detach shared memory
    if (posix_shm_addr != 0 && shmdt(posix_shm_addr) == -1) {
        perror("shmdt failed\n");
    }

    if (write_ptr != 0) {
        fclose(write_ptr);
    }
}

void* get_shmem_posix(int key, size_t size)
{
    // 1. create shared memory
    // int key = 666;
    printf("shm size is %lx\n", size);
    shmid = shmget(key, size, 0666 | IPC_CREAT);
    if (shmid == -1) {
        perror("shmget failed\n");
        exit(EXIT_FAILURE);
    }
    // 2. attach shared memory
    posix_shm_addr = shmat(shmid, NULL, 0);
    if (posix_shm_addr == (void*)-1) {
        perror("shmat failed\n");
        exit(EXIT_FAILURE);
    }
    atexit(posix_shm_exit_handler);
    return posix_shm_addr;
}

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
    ivshm_fd = open("/dev/uio0", O_RDWR);
    assert(ivshm_fd != -1);

    void* ivshm_p = mmap(0, size, PROT_READ | PROT_WRITE, MAP_SHARED, ivshm_fd, 0);
    assert(ivshm_p != NULL);

    atexit(ivshm_exit_handler);
    return ivshm_p;
}

pid_t fork_ops_process(void* shm_addr)
{
    pid_t pid = fork();
    if (pid != 0) { // father
        return pid;
    }
    // child
    // after fork, child inherit all attached shared memory (man shmat)
    printf("waiting on shm_addr %p\n", shm_addr);

    for (int i = 0; i < MAX_DECRYPT_THREAD; i++) {
        args_array[i].index = i;
        args_array[i].inited = false;
        args_array[i].decrypt_status = NONE;
    }

    int counter = 0, non_enc_counter = 0;
    int counters[300] = {};
    // for(int i = 0 ;i < 300; i++){
    // 	counters[i] = 0;
    // }
    std::chrono::duration<double> sum(0);

    BaseRequest* req = (BaseRequest*)shm_addr;
    while (1) {
        if (req->status == EXIT) {
            // decrypt_status = EXIT;
            for (int i = 0; i < MAX_DECRYPT_THREAD; i++) {
                if (args_array[i].inited) {
                    args_array[i].decrypt_status = EXIT;
                }
            }
            std::cout << "SIM-TA Exit: Total ops elapse time " << sum.count() << std::endl;
            printf("ops counter: %d, non-enc counter %d, dec counter: %ld, enc counter: %ld\n", counter, non_enc_counter, decrypt_counter, encrypt_counter);
            for (int i = 0; i <= 299; i++) {
                if (counters[i])
                    printf("%d: counter %d, ", i, counters[i]);
            }
            printf("\n");
            req->status = NONE;
            exit(0);
        } else if (req->status == SENT) {
            LOAD_BARRIER;
            // auto start = std::chrono::system_clock::now();
            // printf("request received %d\n", req->reqType);
            counter++;
            if (req->reqType != CMD_INT_ENC
                && req->reqType != CMD_INT_DEC
                && req->reqType != CMD_FLOAT_ENC
                && req->reqType != CMD_FLOAT_DEC
                && req->reqType != CMD_STRING_ENC
                && req->reqType != CMD_STRING_DEC
                && req->reqType != CMD_TIMESTAMP_ENC
                && req->reqType != CMD_TIMESTAMP_DEC)
                non_enc_counter++;
            counters[req->reqType]++;
            // if (counter % 100000 == 0)
            // 	printf("counter %d\n", counter++);
            if (req->reqType > 0)
                handle_ops(req);
            else
                ;
            // desenitize_ops(req);

            if (req->resp != 0) {
                printf("TA error %d, %d\n", req->resp, counter);
            }
            // auto end = std::chrono::system_clock::now();
            // std::chrono::duration<double> elapsed_seconds = end-start;
            // sum += elapsed_seconds;
            STORE_BARRIER;
            req->status = DONE;
        } else
            YIELD_PROCESSOR;
    }

    // child should never return.
    exit(0);
}

int main(int argc, char* argv[])
{

    int key = 666;
    if (argc == 2) {
        key = atoi(argv[1]);
    }
    printf("sim_server running on shared memory key %d\n", key);
    int data_size = SHM_SIZE;
    pid_t child_pids[20] = {};
#ifdef ENABLE_LOCAL_SIM
    OpsServer* req = (OpsServer*)get_shmem_posix(key, data_size);
#else
    OpsServer* req = (OpsServer*)get_shmem_ivshm(data_size);
#endif
    memset(req, 0, sizeof(OpsServer));
    while (1) {
        if (req->status == SHM_GET) {
            // allocate a empty region
            // printf("processing get\n");
            for (int i = 0; i < MAX_REGION_NUM; i++) {
                if (GET(req->bitmap, i) == 0) {
                    SET(req->bitmap, i);
                    req->ret_id = i;
                    void* addr = (void*)req + REGION_N_OFFSET(i);
                    // printf("allocate %d, base addr %p, alloc addr %p\n",i,req, addr);
                    pid_t child = fork_ops_process(addr);
                    // printf("child pid %d\n",child);
                    child_pids[i] = child;
                    printf("allocate %d, base addr %p, alloc addr %p\n", i, req, addr);
                    break;
                }
            }

            STORE_BARRIER; // store everything before DONE.
            req->status = SHM_DONE;
            // printf("get request done\n");
        } else if (req->status == SHM_FREE) {
            // free a region
            // printf("processing free\n");
            LOAD_BARRIER;
            int id = req->free_id;
            BaseRequest* base_req = (BaseRequest*)((void*)req + REGION_N_OFFSET(id));

            base_req->status = EXIT;
            assert(GET(req->bitmap, id) == 1);
            CLEAR(req->bitmap, id);
            waitpid(child_pids[id], nullptr, 0); // wait until child pid exit;
            // printf("waitpid pid %d\n", child_pids[id]);

            STORE_BARRIER; // store everything before DONE.
            req->status = SHM_DONE;
            printf("free %d, %p\n", id, base_req);
        } else {
            YIELD_PROCESSOR;
        }
    }
}
