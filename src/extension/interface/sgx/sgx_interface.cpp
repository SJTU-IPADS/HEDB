#include <enclave_u.h>
#include <sgx_interface.hpp>
#include <tee_interface.hpp>

#include "sgx_eid.h" /* sgx_enclave_id_t */
#include "sgx_tcrypto.h"
#include "sgx_urts.h"
#include <algorithm>
#include <defs.h>
#include <fstream>
#include <iostream>
#include <thread>
#include <unistd.h>
/* Global EID shared by multiple threads */
sgx_enclave_id_t global_eid = 0;

bool status = false;

// # define TOKEN_FILENAME  "enclave.token"   not impl.
// # define ENCLAVE_FILENAME "enclave.signed.so"  already defined in CMakefile, in install path.
// #define  DATA_FILENAME "hedb.data" already defined in CMakefile, in install path

// FILE *plain_file = 0;
void ocall_print_string(const char* str)
{
    printf(str);
    return;
    // if (plain_file == 0)
    // {
    //     pid_t pid = getpid();
    //     char filename[100];
    //     sprintf(filename, "plain-%d.log", pid);
    //     print_info(filename);
    //     plain_file = fopen(filename,"w+");
    // }
    // fprintf(plain_file, "%s", str);
}

void sgxErrorHandler(int code)
{
    size_t i;
    size_t ttl = sizeof(sgx_errlist) / sizeof(sgx_errlist[0]);
    char ch[1000];
    sprintf(ch, "error code %d\n", code);
    print_info(ch);
    if ((code > 1) || (code < -6)) {
        for (i = 0; i < ttl; i++) {
            if (sgx_errlist[i].err == code)
                print_info("SGX_ERROR_CODE %d: %s \n", code, sgx_errlist[i].msg);
        }
        // ereport(ERROR, (-1, errmsg("SGX_ERROR_CODE: %d \n", code)));
    }

    if (code == -2)
        print_info("SGX_ERROR_CODE %d: ENCLAVE IS NOT RUNNING", code);
    if (code == -3)
        print_info("SGX_ERROR_CODE %d: MEMORY_COPY_ERROR", code);
    if (code == -4)
        print_info("SGX_ERROR_CODE %d: ARITHMETIC_ERROR", code);
    if (code == -5)
        print_info("SGX_ERROR_CODE %d: MEMORY_ALLOCATION_ERROR", code);
    if (code == -6)
        print_info("SGX_ERROR_CODE %d: OUT_OF_THE_RANGE_ERROR", code);
    if (code == -7)
        print_info("INTERFACE_ERROR_CODE %d: BASE64DECODER_ERROR", code);
    if (code == -8)
        print_info("INTERFACE_ERROR_CODE %d: \n The extension was not initialized. Run 'select launch();'", code);
    if (code == -9)
        print_info("INTERFACE_ERROR_CODE %d: \n Cannot open key storage file.'", code);
    if (code == -10)
        print_info("INTERFACE_ERROR_CODE %d: \n The default master key was not set up. Run 'select generate_key();'.", code);
    if (code == TOO_MANY_ELEMENTS_IN_BULK)
        print_info("INTERFACE_ERROR_CODE %d: Too many elements in a bulk for an aggregation function.", code);
}

/* generate if not exist. stored on datafile */
int generateKey()
{

    int resp, resp_enclave, flength;
    uint8_t* sealed_key_b = new uint8_t[SEALED_KEY_LENGTH];

    std::fstream data_file;
    data_file.open(DATA_FILENAME,
        std::fstream::in | std::fstream::out | std::fstream::binary);
    // std::cout << DATA_FILENAME;
    if (data_file) {
        data_file.seekg(0, data_file.end);
        flength = data_file.tellg();

        if (flength == SEALED_KEY_LENGTH)
            return 0;

        else {
            resp = generateKeyEnclave(
                global_eid, &resp_enclave, sealed_key_b, SEALED_KEY_LENGTH);
            if (resp != SGX_SUCCESS)
                return resp;
            data_file.write((char*)sealed_key_b, SEALED_KEY_LENGTH);
        }
    } else
        return NO_KEYS_STORAGE;

    data_file.close();
    delete[] sealed_key_b;

    return (int)flength / SEALED_KEY_LENGTH;
}

int loadKey(int item)
{
    int resp, resp_enclave;
    uint8_t sealed_key_b[SEALED_KEY_LENGTH];

    std::fstream data_file;
    data_file.open(DATA_FILENAME, std::fstream::in | std::fstream::binary);
    if (data_file) {
        data_file.seekg(0, data_file.end);
        int flength = data_file.tellg();
        if (flength < item * SEALED_KEY_LENGTH + SEALED_KEY_LENGTH)
            return NO_KEY_ID;

        data_file.seekg(item * SEALED_KEY_LENGTH);
        data_file.read((char*)sealed_key_b, SEALED_KEY_LENGTH);
        resp = loadKeyEnclave(
            global_eid, &resp_enclave, sealed_key_b, SEALED_KEY_LENGTH);
        if (resp != SGX_SUCCESS)
            return resp;
    } else
        return NO_KEYS_STORAGE;

    data_file.close();
    return 0;
}

int launch_enclave()
{
    sgx_launch_token_t token = { 0 };
    int updated = 0;
    int resp = sgx_create_enclave(
        ENCLAVE_FILENAME, SGX_DEBUG_FLAG, &token, &updated, &global_eid, NULL);
    // print_info(ENCLAVE_FILENAME);
    return resp;
}

void enclaveThread(void* buffer)
{
    int resp = 0;
    // print_info("into enclave thread\n");
    resp = enclaveProcess(global_eid, &resp, buffer);
    // print_info("enclave return\n");
    char ch[1000];
    sprintf(ch, "enclave process should not return %d", resp);
    print_info(ch);
    exit(0);
}

/* --------------------------------------------- */
void* getSharedBuffer(size_t size)
{
    void* buffer = malloc(size);
    int resp = launch_enclave();
    if (resp != 0)
        sgxErrorHandler(resp);
    resp = generateKey();
    if (resp != 0)
        sgxErrorHandler(resp);
    resp = loadKey(0);
    if (resp != 0)
        sgxErrorHandler(resp);
    std::thread th = std::thread(&enclaveThread, buffer);
    th.detach();

    return buffer;
}
void freeBuffer(void* buffer)
{
    /* stop enclave first TODO */
    free(buffer);
}
