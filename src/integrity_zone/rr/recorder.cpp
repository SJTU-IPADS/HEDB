#include "stdafx.hpp"

#include <recorder.hpp>

#include <request.hpp>
#include <request_types.h>

#include <rrprintf.hpp>
#include <assert.h>

static inline uint64_t get_timestamp(void)
{
    uint64_t tsc;
#if defined(__aarch64__) && 0 // this can be opened if kernel enables user space read of pmccntr reg.
    asm volatile("mrs %0, pmccntr_el0" : "=r"(tsc));
#elif defined(__x86_64__)
	unsigned int a, d;
	asm volatile("rdtscp" : "=a" (a), "=d" (d) : : "%rbx", "%rcx");
	tsc = ((unsigned long) a) | (((unsigned long) d) << 32);
#else
    tsc = 0;
#endif
    return tsc;
}

void Recorder::update_write_fd(std::string filename_prefix)
{
    if (write_fd) {
        close(write_fd);
    }
    file_length = 0;
    file_cursor = 0;
    write_addr = nullptr;
    pid_t pid = getpid();
    if (filename_prefix == "") {
        filename = "record-" + std::to_string(pid) + ".log";
    } else {
        filename = filename_prefix + "-" + std::to_string(pid) + ".log";
    }
    write_fd = open(filename.c_str(), O_RDWR | O_CREAT, 0666);
}

char* Recorder::get_write_buffer(unsigned long length)
{
    assert(write_fd != 0);
    if (file_cursor + length > file_length) {
        // munmap(write_addr, file_length);
        file_length += DATA_LENGTH;
        ftruncate(write_fd, file_length);
        write_addr = (char*)mmap(NULL, file_length, PROT_READ | PROT_WRITE, MAP_SHARED, write_fd, 0);
        madvise(write_addr + file_cursor, DATA_LENGTH, MADV_SEQUENTIAL);
    }

    char* start = write_addr + file_cursor;
    file_cursor += length;
    return start;
}

void Recorder::record(void* req_buffer)
{
    /* left~ to be restructured to class member function */
    BaseRequest* req_control = static_cast<BaseRequest*>(req_buffer);
    if (req_control->reqType != CMD_FLOAT_ENC
        && req_control->reqType != CMD_FLOAT_DEC
        && req_control->reqType >= CMD_FLOAT_PLUS
        && req_control->reqType <= CMD_FLOAT_SUM_BULK) {
        if (req_control->reqType == CMD_FLOAT_CMP) {
            EncFloatCmpRequestData* req = (EncFloatCmpRequestData*)req_buffer;
            size_t length = sizeof(int) * 3 + ENC_FLOAT4_LENGTH * 2 + sizeof(uint64_t);
            char* dst = get_write_buffer(length);
            uint64_t timestamp = get_timestamp();
            rrprintf(1, dst, 6, 
                sizeof(int), &req_control->reqType,
                ENC_FLOAT4_LENGTH, &req->left,
                ENC_FLOAT4_LENGTH, &req->right,
                sizeof(int), &req->cmp,
                sizeof(int), &req_control->resp,
                sizeof(uint64_t), &timestamp);
        } else if (req_control->reqType == CMD_FLOAT_SUM_BULK) {
            EncFloatBulkRequestData* req = (EncFloatBulkRequestData*)req_buffer;
            size_t length = sizeof(int) * 3 + ENC_FLOAT4_LENGTH + sizeof(uint64_t);
            char* dst = get_write_buffer(length);
            uint64_t timestamp = get_timestamp();
            rrprintf(1, dst, 5,
                sizeof(int), &req_control->reqType,
                sizeof(int), &req->bulk_size,
                ENC_FLOAT4_LENGTH, &req->res,
                sizeof(int), &req_control->resp,
                sizeof(uint64_t), &timestamp);

            dst = get_write_buffer(req->bulk_size * ENC_FLOAT4_LENGTH);
            for (int i = 0; i < req->bulk_size; i++) {
                memcpy(dst, &req->items[i], ENC_FLOAT4_LENGTH);
                dst += ENC_FLOAT4_LENGTH;
            }
        } else {
            EncFloatCalcRequestData* req = (EncFloatCalcRequestData*)req_buffer;
            size_t length = sizeof(int) * 2 + ENC_FLOAT4_LENGTH * 3 + sizeof(uint64_t);
            char* dst = get_write_buffer(length);
            uint64_t timestamp = get_timestamp();
            rrprintf(1, dst, 6, 
                sizeof(int), &req->op,
                ENC_FLOAT4_LENGTH, &req->left,
                ENC_FLOAT4_LENGTH, &req->right,
                ENC_FLOAT4_LENGTH, &req->res,
                sizeof(int), &req_control->resp,
                sizeof(uint64_t), &timestamp);
        }

    } // end of float
    else if (req_control->reqType >= CMD_INT_PLUS
        && req_control->reqType <= CMD_INT_SUM_BULK
        && req_control->reqType != CMD_INT_ENC
        && req_control->reqType != CMD_INT_DEC) {
        if (req_control->reqType == CMD_INT_CMP) {
            EncIntCmpRequestData* req = (EncIntCmpRequestData*)req_buffer;
            size_t length = sizeof(int) * 3 + ENC_INT32_LENGTH * 2 + sizeof(uint64_t);
            char* dst = get_write_buffer(length);
            uint64_t timestamp = get_timestamp();
            rrprintf(1, dst, 6,
                sizeof(int), &req_control->reqType,
                ENC_INT32_LENGTH, &req->left,
                ENC_INT32_LENGTH, &req->right,
                sizeof(int), &req->cmp,
                sizeof(int), &req_control->resp,
                sizeof(uint64_t), &timestamp);
        } else if (req_control->reqType == CMD_INT_SUM_BULK) {
            EncIntBulkRequestData* req = (EncIntBulkRequestData*)req_buffer;
            size_t length = sizeof(int) * 3 + ENC_INT32_LENGTH + sizeof(uint64_t);
            char* dst = get_write_buffer(length);
            uint64_t timestamp = get_timestamp();
            rrprintf(1, dst, 5,
                sizeof(int), &req_control->reqType,
                sizeof(int), &req->bulk_size,
                ENC_INT32_LENGTH, &req->res,
                sizeof(int), &req_control->resp,
                sizeof(uint64_t), &timestamp);

            dst = get_write_buffer(req->bulk_size * ENC_INT32_LENGTH);
            for (int i = 0; i < req->bulk_size; i++) {
                memcpy(dst, &req->items[i], ENC_INT32_LENGTH);
                dst += ENC_INT32_LENGTH;
            }
        } else {
            EncIntCalcRequestData* req = (EncIntCalcRequestData*)req_buffer;
            size_t length = sizeof(int) * 2 + ENC_INT32_LENGTH * 3 + sizeof(uint64_t);
            char* dst = get_write_buffer(length);
            uint64_t timestamp = get_timestamp();
            rrprintf(1, dst, 6, 
                sizeof(int), &req->op,
                ENC_INT32_LENGTH, &req->left,
                ENC_INT32_LENGTH, &req->right,
                ENC_INT32_LENGTH, &req->res,
                sizeof(int), &req_control->resp,
                sizeof(uint64_t), &timestamp);
        }
    } else if (req_control->reqType != CMD_STRING_ENC
        && req_control->reqType != CMD_STRING_DEC
        && req_control->reqType >= CMD_STRING_CMP
        && req_control->reqType <= CMD_STRING_LIKE) {
        if (req_control->reqType == CMD_STRING_CMP || req_control->reqType == CMD_STRING_LIKE) {
            EncStrCmpRequestData* req = (EncStrCmpRequestData*)req_buffer;
            int left_length = encstr_size(req->left);
            int right_length = encstr_size(req->right);
            int length = sizeof(int) * 5 + left_length + right_length + sizeof(uint64_t);
            char* dst = get_write_buffer(length);
            uint64_t timestamp = get_timestamp();
            rrprintf(1, dst, 8,
                sizeof(int), &req_control->reqType,
                sizeof(int), &left_length,
                sizeof(int), &right_length,
                left_length, &req->left,
                right_length, &req->right,
                sizeof(int), &req->cmp,
                sizeof(int), &req_control->resp,
                sizeof(uint64_t), &timestamp);
        } else if (req_control->reqType == CMD_STRING_SUBSTRING) {
            SubstringRequestData* req = (SubstringRequestData*)req_buffer;
            int str_length = encstr_size(req->str), result_length = encstr_size(req->res);
            size_t length = sizeof(int) * 4 + str_length + result_length + 2 * sizeof(int32_t) + sizeof(uint64_t);
            char* dst = get_write_buffer(length);
            uint64_t timestamp = get_timestamp();
            rrprintf(1, dst, 9,
                sizeof(int), &req_control->reqType,
                sizeof(int), &str_length,
                sizeof(int), &result_length,
                str_length, &req->str,
                sizeof(int32_t), &req->start,
                sizeof(int32_t), &req->length,
                result_length, &req->res,
                sizeof(int), &req_control->resp,
                sizeof(uint64_t), &timestamp);
        } else {
            EncStrCalcRequestData* req = (EncStrCalcRequestData*)req_buffer;
            int left_length = encstr_size(req->left), right_length = encstr_size(req->right), res_length = encstr_size(req->res);
            int length = sizeof(int) * 5 + left_length + right_length + res_length + sizeof(uint64_t);
            char* dst = get_write_buffer(length);
            uint64_t timestamp = get_timestamp();
            rrprintf(1, dst, 9,
                sizeof(int), &req->op,
                sizeof(int), &left_length,
                sizeof(int), &right_length,
                sizeof(int), &res_length,
                left_length, &req->left,
                right_length, &req->right,
                res_length, &req->res,
                sizeof(int), &req_control->resp,
                sizeof(uint64_t), &timestamp);
        }
    } else if (req_control->reqType >= CMD_TIMESTAMP_CMP
        && req_control->reqType <= CMD_TIMESTAMP_EXTRACT_YEAR
        && req_control->reqType != CMD_TIMESTAMP_DEC
        && req_control->reqType != CMD_TIMESTAMP_ENC) {
        if (req_control->reqType == CMD_TIMESTAMP_CMP) {
            EncTimestampCmpRequestData* req = (EncTimestampCmpRequestData*)req_buffer;
            int length = sizeof(int) * 3 + ENC_TIMESTAMP_LENGTH * 2 + sizeof(uint64_t);
            char* dst = get_write_buffer(length);
            uint64_t timestamp = get_timestamp();
            rrprintf(1, dst, 6, 
                sizeof(int), &req_control->reqType,
                ENC_TIMESTAMP_LENGTH, &req->left,
                ENC_TIMESTAMP_LENGTH, &req->right,
                sizeof(int), &req->cmp,
                sizeof(int), &req_control->resp,
                sizeof(uint64_t), &timestamp);
        } else if (req_control->reqType == CMD_TIMESTAMP_EXTRACT_YEAR) {
            EncTimestampExtractYearRequestData* req = (EncTimestampExtractYearRequestData*)req_buffer;
            size_t length = sizeof(int) * 2 + ENC_TIMESTAMP_LENGTH + ENC_INT32_LENGTH + sizeof(uint64_t);
            char* dst = get_write_buffer(length);
            uint64_t timestamp = get_timestamp();
            rrprintf(1, dst, 5,
                sizeof(int), &req_control->reqType,
                ENC_TIMESTAMP_LENGTH, &req->in,
                ENC_INT32_LENGTH, &req->res,
                sizeof(int), &req_control->resp,
                sizeof(uint64_t), &timestamp);
        }
    }
}

