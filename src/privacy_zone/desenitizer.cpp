// SPDX-License-Identifier: Mulan PSL v2
/*
 * Copyright (c) 2021 - 2023 The HEDB Project.
 */

#include <assert.h>
#include <cmath>
#include <desenitizer.h>
#include <pthread.h>
#include <request_types.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <time.h>
#include <unistd.h>

#include <map>
#include <queue>
using namespace std;

#include <crypto.h>
#include <stdlib.h>

#if defined(TEE_CVM)
#include "ops_server.hpp"
#endif

/* kTest utils */
#define KTEST_MAGIC "KTEST"
#define KTEST_MAGIC_SIZE 5
#define KTEST_VERSION 3

typedef struct KTestObject KTestObject;
struct KTestObject {
    char* name;
    unsigned numBytes;
    unsigned char* bytes;
};

typedef struct KTest KTest;
struct KTest {
    /* file format version */
    unsigned version;

    unsigned numArgs;
    char** args;

    unsigned symArgvs;
    unsigned symArgvLen;

    unsigned numObjects;
    KTestObject* objects;
};

static int write_uint32(FILE* f, unsigned value)
{
    unsigned char data[4];
    data[0] = value >> 24;
    data[1] = value >> 16;
    data[2] = value >> 8;
    data[3] = value >> 0;
    return fwrite(data, 1, 4, f) == 4;
}

static int write_string(FILE* f, const char* value)
{
    unsigned len = strlen(value);
    if (!write_uint32(f, len))
        return 0;
    if (fwrite(value, len, 1, f) != 1)
        return 0;
    return 1;
}

int kTest_toFile(KTest* bo, const char* path)
{
    FILE* f = fopen(path, "wb");
    unsigned i;

    if (!f)
        goto error;
    if (fwrite(KTEST_MAGIC, strlen(KTEST_MAGIC), 1, f) != 1)
        goto error;
    if (!write_uint32(f, KTEST_VERSION))
        goto error;

    if (!write_uint32(f, bo->numArgs))
        goto error;
    for (i = 0; i < bo->numArgs; i++) {
        if (!write_string(f, bo->args[i]))
            goto error;
    }

    if (!write_uint32(f, bo->symArgvs))
        goto error;
    if (!write_uint32(f, bo->symArgvLen))
        goto error;

    if (!write_uint32(f, bo->numObjects))
        goto error;
    for (i = 0; i < bo->numObjects; i++) {
        KTestObject* o = &bo->objects[i];
        if (!write_string(f, o->name))
            goto error;
        if (!write_uint32(f, o->numBytes))
            goto error;
        if (fwrite(o->bytes, o->numBytes, 1, f) != 1)
            goto error;
    }

    fclose(f);

    return 1;
error:
    if (f)
        fclose(f);

    return 0;
}

/* use following api to generate a ktest file */

#define MAX 64
static void push_obj(KTest* b, const char* name, unsigned total_bytes,
    unsigned char* bytes)
{
    KTestObject* o = &b->objects[b->numObjects++];
    assert(b->numObjects < MAX);

    o->name = strdup(name);
    o->numBytes = total_bytes;
    o->bytes = (unsigned char*)malloc(o->numBytes);

    memcpy(o->bytes, bytes, total_bytes);
}

static void push_range(KTest* b, const char* name, unsigned value)
{
    KTestObject* o = &b->objects[b->numObjects++];
    assert(b->numObjects < MAX);

    o->name = strdup(name);
    o->numBytes = 4;
    o->bytes = (unsigned char*)malloc(o->numBytes);

    *(unsigned*)o->bytes = value;
}

static void push_int(KTest* b, const char* name, int value)
{
    push_obj(b, name, 4, (unsigned char*)&value);
}

static void push_string(KTest* b, const char* name, int len, const char* str)
{
    push_obj(b, name, len, (unsigned char*)str);
}

int decrypt_bytes(uint8_t* pSrc, size_t src_len, uint8_t* pDst, size_t exp_dst_len)
{
    size_t dst_len = 0;
    int resp = 0;
    if (src_len <= IV_SIZE + TAG_SIZE) {
        printf("error src len");
        return -1;
    }
    resp = gcm_decrypt(pSrc, src_len, pDst, &dst_len);

    return resp;
}

#define STR_SYMBOL_SIZE 500

uint32_t max_str_len = 0;
int gen_ktest_file(BaseRequest* base_req, const char* name)
{
    // base_req->reqType = - base_req->reqType;

    KTest b;
    b.symArgvs = 0;
    b.symArgvLen = 0;
    b.numObjects = 0;
    b.numArgs = 0;
    b.objects = (KTestObject*)malloc(MAX * sizeof *b.objects);
    push_range(&b, "model_version", 1);
    push_int(&b, "op", base_req->reqType);
    // printf("gen ktest %d\n", base_req->reqType);
    switch (base_req->reqType) {
    case CMD_INT_PLUS:
    case CMD_INT_MINUS:
    case CMD_INT_MULT:
    case CMD_INT_DIV:
    case CMD_INT_EXP:
    case CMD_INT_MOD: {
        int left, right;
        EncIntCalcRequestData* req = (EncIntCalcRequestData*)base_req;
        decrypt_bytes((uint8_t*)&req->left, sizeof(req->left), (uint8_t*)&left, sizeof(left));
        decrypt_bytes((uint8_t*)&req->right, sizeof(req->right), (uint8_t*)&right, sizeof(right));
        push_int(&b, "int_calc_type", base_req->reqType);
        push_int(&b, "int_calc_left", left);
        push_int(&b, "int_calc_right", right);
        break;
    }
    case CMD_INT_CMP: {
        int left, right;
        EncIntCmpRequestData* req = (EncIntCmpRequestData*)base_req;
        decrypt_bytes((uint8_t*)&req->left, sizeof(req->left), (uint8_t*)&left, sizeof(left));
        decrypt_bytes((uint8_t*)&req->right, sizeof(req->right), (uint8_t*)&right, sizeof(right));
        push_int(&b, "int_cmp_left", left);
        push_int(&b, "int_cmp_right", right);
        break;
    }
    case CMD_INT_SUM_BULK: {
        EncIntBulkRequestData* req = (EncIntBulkRequestData*)base_req;
        int bulk_type = CMD_INT_SUM_BULK, size = req->bulk_size;
        int* array = (int*)malloc(size * sizeof(int));
        for (int i = 0; i < size; i++) {
            decrypt_bytes((uint8_t*)&(req->items[i]), sizeof(req->items[0]), (uint8_t*)&array[i], sizeof(int));
        }
        push_int(&b, "int_bulk_type", bulk_type);
        push_int(&b, "int_bulk_size", size);
        push_obj(&b, "int_bulk_array", BULK_SIZE * sizeof(int), (unsigned char*)array);
        break;
    }
        /* FLOAT are not supported currrently, use int to repalce float */

    case CMD_FLOAT_PLUS:
    case CMD_FLOAT_MINUS:
    case CMD_FLOAT_MULT:
    case CMD_FLOAT_DIV:
    case CMD_FLOAT_EXP:
    case CMD_FLOAT_MOD: {
        EncFloatCalcRequestData* req = (EncFloatCalcRequestData*)base_req;
        float left, right;
        decrypt_bytes((uint8_t*)&req->left, sizeof(req->left), (uint8_t*)&left, sizeof(left));
        decrypt_bytes((uint8_t*)&req->right, sizeof(req->right), (uint8_t*)&right, sizeof(right));
        push_int(&b, "int_calc_type", base_req->reqType - 100);
        push_int(&b, "int_calc_left", (int)left);
        push_int(&b, "int_calc_right", (int)right);

        break;
    }

    case CMD_FLOAT_CMP: {
        float left, right;
        EncFloatCmpRequestData* req = (EncFloatCmpRequestData*)base_req;
        decrypt_bytes((uint8_t*)&req->left, sizeof(req->left), (uint8_t*)&left, sizeof(left));
        decrypt_bytes((uint8_t*)&req->right, sizeof(req->right), (uint8_t*)&right, sizeof(right));
        push_int(&b, "int_cmp_left", (int)left);
        push_int(&b, "int_cmp_right", (int)right);
        break;
    }
    case CMD_FLOAT_SUM_BULK: {
        EncFloatBulkRequestData* req = (EncFloatBulkRequestData*)base_req;
        int bulk_type = CMD_INT_SUM_BULK, size = req->bulk_size; // use int_sum_bulk to repalce float sum bulk
        int* array = (int*)malloc(size * sizeof(int));
        float tmp;
        for (int i = 0; i < size; i++) {
            decrypt_bytes((uint8_t*)&(req->items[i]), sizeof(req->items[0]), (uint8_t*)&tmp, sizeof(tmp));
            array[i] = tmp;
        }
        push_int(&b, "int_bulk_type", bulk_type);
        push_int(&b, "int_bulk_size", size);
        push_obj(&b, "int_bulk_array", BULK_SIZE * sizeof(int), (unsigned char*)array);
        break;
    }
    case CMD_FLOAT_EVAL_EXPR:
        /* not implemented */
        break;

    /* timestamp*/
    case CMD_TIMESTAMP_EXTRACT_YEAR: {
        EncTimestampExtractYearRequestData* req = (EncTimestampExtractYearRequestData*)base_req;
        TIMESTAMP t;
        decrypt_bytes((uint8_t*)&req->in, sizeof(req->in), (uint8_t*)&t, sizeof(t));
        push_obj(&b, "timestamp_extract_year_t", sizeof(t), (unsigned char*)&t);
        break;
    }
    case CMD_TIMESTAMP_CMP: {

        EncTimestampCmpRequestData* req = (EncTimestampCmpRequestData*)base_req;
        TIMESTAMP left, right;
        decrypt_bytes((uint8_t*)&req->left, sizeof(req->left), (uint8_t*)&left, sizeof(left));
        decrypt_bytes((uint8_t*)&req->right, sizeof(req->right), (uint8_t*)&right, sizeof(right));
        push_obj(&b, "timestamp_cmp_left", sizeof(left), (unsigned char*)&left);
        push_obj(&b, "timestamp_cmp_right", sizeof(right), (unsigned char*)&right);
        break;

    } /* text */

    case CMD_STRING_SUBSTRING: {
        SubstringRequestData* req = (SubstringRequestData*)base_req;
        Str str;
        int start, length;
        str.len = req->str.len - IV_SIZE - TAG_SIZE;
        decrypt_bytes((uint8_t*)&req->str.enc_cstr, req->str.len, (uint8_t*)&str.data, str.len);
        decrypt_bytes((uint8_t*)&req->start, sizeof(req->start), (uint8_t*)&start, sizeof(start));
        decrypt_bytes((uint8_t*)&req->length, sizeof(req->length), (uint8_t*)&length, sizeof(length));
        str.data[str.len] = '\0';
        push_string(&b, "text_substring_str", STR_SYMBOL_SIZE, (const char*)str.data);
        push_int(&b, "text_substring_start", start);
        push_int(&b, "text_substring_length", length);
        max_str_len = max(max_str_len, str.len);
        break;
    }
    case CMD_STRING_CONCAT: {
        EncStrCalcRequestData* req = (EncStrCalcRequestData*)base_req;
        Str left, right;
        left.len = req->left.len - IV_SIZE - TAG_SIZE;
        right.len = req->right.len - IV_SIZE - TAG_SIZE;
        decrypt_bytes((uint8_t*)&req->left.enc_cstr, req->left.len, (uint8_t*)&left.data, left.len);
        decrypt_bytes((uint8_t*)&req->right.enc_cstr, req->right.len, (uint8_t*)&right.data, right.len);
        left.data[left.len] = '\0';
        right.data[right.len] = '\0';
        push_string(&b, "text_concat_left", STR_SYMBOL_SIZE, (const char*)left.data);
        push_string(&b, "text_concat_right", STR_SYMBOL_SIZE, (const char*)right.data);
        max_str_len = max(max_str_len, left.len);
        max_str_len = max(max_str_len, right.len);
        break;
    }
    case CMD_STRING_LIKE:
    case CMD_STRING_CMP: {
        EncStrCmpRequestData* req = (EncStrCmpRequestData*)base_req;
        Str left, right;
        left.len = req->left.len - IV_SIZE - TAG_SIZE;
        right.len = req->right.len - IV_SIZE - TAG_SIZE;
        decrypt_bytes((uint8_t*)&req->left.enc_cstr, req->left.len, (uint8_t*)&left.data, left.len);
        decrypt_bytes((uint8_t*)&req->right.enc_cstr, req->right.len, (uint8_t*)&right.data, right.len);
        left.data[left.len] = '\0';
        right.data[right.len] = '\0';
        push_string(&b, "text_cmp_left", STR_SYMBOL_SIZE, (const char*)left.data);
        push_string(&b, "text_cmp_right", STR_SYMBOL_SIZE, (const char*)right.data);
        max_str_len = max(max_str_len, left.len);
        max_str_len = max(max_str_len, right.len);
        break;
    }
    default:
        break;
    }

    if (!kTest_toFile(&b, name))
        assert(0);

    for (int i = 0; i < (int)b.numObjects; ++i) {
        free(b.objects[i].name);
        free(b.objects[i].bytes);
    }
    free(b.objects);

    return 0;
}
// #define KTEST_FILEPATH "gen.ktest"
int gen_tmp_constraints(BaseRequest* base_req, const char* filename, int line_id)
{
    gen_ktest_file(base_req, filename);

    char cmd[100];
    sprintf(cmd, "../klee_scripts/gen_constraint.sh %s %d", filename, line_id);
    system(cmd);
    return 0;
}

static int retrieve_request_from_file(FILE* f, char* req_buffer)
{
    char read_buffer[sizeof(EncIntBulkRequestData)];
    int op;

    int bytes = fread(&op, 1, sizeof(op), f);
    // char ch = getchar();
    if (bytes < 1) {
        printf("eof, exiting\n");
        return -1;
    } else if (op == 0) {
        printf("no new ops, exiting\n");
        return -2;
    }
    BaseRequest* br = (BaseRequest*)req_buffer;
    br->reqType = op;
    switch (op) { // here each buffer should decrease by an int(which is op, that is read earlier)
    case CMD_INT_CMP: {
        EncIntCmpRequestData* req = (EncIntCmpRequestData*)req_buffer;
        char* src = read_buffer;
        fread(read_buffer, sizeof(char), sizeof(int) * 2 + ENC_INT32_LENGTH * 2 + sizeof(uint64_t), f);
        memcpy(&req->left, src, ENC_INT32_LENGTH);
        src += ENC_INT32_LENGTH;
        memcpy(&req->right, src, ENC_INT32_LENGTH);
        src += ENC_INT32_LENGTH;
        break;
    }
    case CMD_INT_SUM_BULK: {
        EncIntBulkRequestData* req = (EncIntBulkRequestData*)req_buffer;
        char* src = read_buffer;
        fread(read_buffer, sizeof(char), sizeof(int) * 2 + ENC_INT32_LENGTH + sizeof(uint64_t), f);
        memcpy(&req->bulk_size, src, sizeof(int));
        src += sizeof(int);
        fread(read_buffer, sizeof(char), req->bulk_size * ENC_INT32_LENGTH, f);
        src = read_buffer;
        for (int i = 0; i < req->bulk_size; i++) {
            memcpy(&req->items[i], src, ENC_INT32_LENGTH);
            src += ENC_INT32_LENGTH;
        }
        break;
    }
    case CMD_INT_PLUS:
    case CMD_INT_MINUS:
    case CMD_INT_MULT:
    case CMD_INT_DIV:
    case CMD_INT_EXP:
    case CMD_INT_MOD: {
        EncIntCalcRequestData* req = (EncIntCalcRequestData*)req_buffer;
        char* src = read_buffer;
        fread(read_buffer, sizeof(char), sizeof(int) + ENC_INT32_LENGTH * 3 + sizeof(uint64_t), f);
        memcpy(&req->left, src, ENC_INT32_LENGTH);
        src += ENC_INT32_LENGTH;
        memcpy(&req->right, src, ENC_INT32_LENGTH);
        src += ENC_INT32_LENGTH;
        break;
    }
    /* FLOAT are not supported currrently */
    case CMD_FLOAT_PLUS:
    case CMD_FLOAT_MINUS:
    case CMD_FLOAT_MULT:
    case CMD_FLOAT_DIV:
    case CMD_FLOAT_EXP:
    case CMD_FLOAT_MOD: {
        EncFloatCalcRequestData* req = (EncFloatCalcRequestData*)req_buffer;
        char* src = read_buffer;
        fread(read_buffer, sizeof(char), sizeof(int) + ENC_FLOAT4_LENGTH * 3 + sizeof(uint64_t), f);
        memcpy(&req->left, src, ENC_FLOAT4_LENGTH);
        src += ENC_FLOAT4_LENGTH;
        memcpy(&req->right, src, ENC_FLOAT4_LENGTH);
        break;
    }
    case CMD_FLOAT_CMP: {
        EncFloatCmpRequestData* req = (EncFloatCmpRequestData*)req_buffer;
        char* src = read_buffer;
        fread(read_buffer, sizeof(char), sizeof(int) * 2 + ENC_FLOAT4_LENGTH * 2 + sizeof(uint64_t), f);
        memcpy(&req->left, src, ENC_FLOAT4_LENGTH);
        src += ENC_FLOAT4_LENGTH;
        memcpy(&req->right, src, ENC_FLOAT4_LENGTH);
        src += ENC_FLOAT4_LENGTH;
        break;
    }
    case CMD_FLOAT_SUM_BULK: {
        EncFloatBulkRequestData* req = (EncFloatBulkRequestData*)req_buffer;
        char* src = read_buffer;
        fread(read_buffer, sizeof(char), sizeof(int) * 2 + ENC_FLOAT4_LENGTH + sizeof(uint64_t), f);
        memcpy(&req->bulk_size, src, sizeof(int));
        src += sizeof(int);

        src = read_buffer;
        fread(read_buffer, sizeof(char), req->bulk_size * ENC_FLOAT4_LENGTH, f);
        for (int i = 0; i < req->bulk_size; i++) {
            memcpy(&req->items[i], src, ENC_FLOAT4_LENGTH);
            src += ENC_FLOAT4_LENGTH;
        }
        break;
    }
    case CMD_STRING_LIKE:
    case CMD_STRING_CMP: {
        EncStrCmpRequestData* req = (EncStrCmpRequestData*)req_buffer;
        int left_length, right_length;
        fread(&left_length, 1, sizeof(int), f);
        fread(&right_length, 1, sizeof(int), f);

        char* src = read_buffer;
        fread(read_buffer, sizeof(char), left_length + right_length + sizeof(int) * 2 + sizeof(uint64_t), f);
        memcpy(&req->left, src, left_length);
        src += left_length;
        memcpy(&req->right, src, right_length);
        src += right_length;
        break;
    }
    case CMD_STRING_SUBSTRING: {
        SubstringRequestData* req = (SubstringRequestData*)req_buffer;
        int str_length, result_length;
        fread(&str_length, sizeof(int), 1, f);
        fread(&result_length, sizeof(int), 1, f);
        char* src = read_buffer;
        fread(read_buffer, sizeof(char), str_length + result_length + sizeof(int) + ENC_INT32_LENGTH * 2 + sizeof(uint64_t), f);
        memcpy(&req->str, src, str_length);
        src += str_length;
        memcpy(&req->start, src, ENC_INT32_LENGTH);
        src += ENC_INT32_LENGTH;
        memcpy(&req->length, src, ENC_INT32_LENGTH);
        src += ENC_INT32_LENGTH;
        break;
    }
    case CMD_STRING_CONCAT: {
        EncStrCalcRequestData* req = (EncStrCalcRequestData*)req_buffer;
        int left_length, right_length, res_length;
        fread(&left_length, sizeof(int), 1, f);
        fread(&right_length, sizeof(int), 1, f);
        fread(&res_length, sizeof(int), 1, f);

        fread(read_buffer, sizeof(char), sizeof(int) + left_length + right_length + res_length + sizeof(uint64_t), f);
        char* src = read_buffer;
        memcpy(&req->left, src, left_length);
        src += left_length;
        memcpy(&req->right, src, right_length);
        src += right_length;
        break;
    }
    case CMD_TIMESTAMP_CMP: {
        EncTimestampCmpRequestData* req = (EncTimestampCmpRequestData*)req_buffer;
        fread(read_buffer, sizeof(char), sizeof(int) * 2 + ENC_TIMESTAMP_LENGTH * 2 + sizeof(uint64_t), f);
        char* src = read_buffer;
        memcpy(&req->left, src, ENC_TIMESTAMP_LENGTH);
        src += ENC_TIMESTAMP_LENGTH;
        memcpy(&req->right, src, ENC_TIMESTAMP_LENGTH);
        src += ENC_TIMESTAMP_LENGTH;
        break;
    }
    case CMD_TIMESTAMP_EXTRACT_YEAR: {
        EncTimestampExtractYearRequestData* req = (EncTimestampExtractYearRequestData*)req_buffer;
        char read_buffer[sizeof(int) + ENC_TIMESTAMP_LENGTH + ENC_INT32_LENGTH + sizeof(uint64_t)];
        fread(read_buffer, sizeof(char), sizeof(int) + ENC_TIMESTAMP_LENGTH + ENC_INT32_LENGTH + sizeof(uint64_t), f);
        char* src = read_buffer;
        memcpy(&req->in, src, ENC_TIMESTAMP_LENGTH);
        src += ENC_TIMESTAMP_LENGTH;
        break;
    }

    case CMD_FLOAT_EVAL_EXPR:
    default:
        printf("unknown ops, critical error\n");
        exit(0);
        break;
    }
    return 0;
}

pthread_mutex_t mutex;
int recnum = 0;
FILE* f;
const char* outfile_suffix = "desen.log";
void* thread_entry(void* arg)
{
    // int id = (long long)arg;
    char req_buffer[sizeof(EncIntBulkRequestData)];
    BaseRequest* br = (BaseRequest*)req_buffer;

    char ktest_file[30];
    // sprintf(ktest_file, "%d-gen.ktest", id);
    int cur_rec_num;
    while (1) {
        // pthread_mutex_lock(&mutex);
        int resp = retrieve_request_from_file(f, req_buffer);
        cur_rec_num = recnum++;
        // pthread_mutex_unlock(&mutex);

        if (resp < 0) {
            if (resp == -2)
                printf("retrieve request failed %d, max strlen %d\n", resp, max_str_len);
            break;
        }
        if (br->reqType > 100 && br->reqType <= 110) {
            continue; // float just skip
        }
        // if(br->reqType < 200)
        //     continue;
        sprintf(ktest_file, "ktests/%d-gen.ktest", cur_rec_num);
        gen_ktest_file((BaseRequest*)req_buffer, ktest_file);
        // gen_tmp_constraints((BaseRequest *)req_buffer, ktest_file);

        /**
         * priority_queue [recnum, filename]
         *
         * producer:
         * while(1){
         * lock()
         * add_to_priority_queue()
         * unlock()
         * sig()
         * wait_for_processing()
         * }
         *
         * consumer:
         * while(1){
         *   lock()
         *   while(priority_queue.front() != cur_pos)
         *     wait_sig()
         *   do_following_work
         * }
         * map: hash -> string(output)
         * if(!map.has[hash])
         *     python gen => insert into map
         * fprintf(map[hash])
         *
         */
    }
    return 0;
}

#define NUM_THREAD 8
int main(int argc, char* argv[])
{
    assert(argc == 2);
    f = fopen(argv[1], "r+");
    thread_entry(0);
    // pthread_mutex_init(&mutex, NULL);
    // pthread_t pids[NUM_THREAD];
    // for(long long i = 0; i < NUM_THREAD; i++){
    //     if(pthread_create(&pids[i], NULL, thread_entry, (void *)i) != 0){
    //         printf("pthread create failed\n");
    //         return -1;
    //     }
    // }
    // for(int i = 0; i < NUM_THREAD; i ++){
    //     pthread_join(pids[i], NULL);
    // }
    // pthread_mutex_destroy(&mutex);
    fclose(f);
    return 0;
}
