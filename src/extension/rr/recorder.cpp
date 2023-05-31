#include <recorder.hpp>

#include <request.hpp>
#include <request_types.h>


static inline uint64_t get_timestamp(void){
    uint64_t tsc;
#if defined(__aarch64__) && 0 // this can be opened if kernel enables user space read of pmccntr reg.
    asm volatile("mrs %0, pmccntr_el0"
                 : "=r"(tsc));
#else
    tsc = 0;
#endif
    return tsc;
}

char *Recorder::get_write_buffer(unsigned long length){
    if(write_fd == 0){
        pid_t pid = getpid();
        if(prefix == ""){
            filename = "record-" + std::to_string(pid) + ".log";
        } else {
            filename = prefix + "-" + std::to_string(pid) + ".log";
        }
        write_fd = open(filename.c_str(), O_RDWR | O_CREAT, 0666);
    }
    if (file_cursor + length > file_length) {
        // munmap(write_addr, file_length);
        file_length += DATA_LENGTH;
        ftruncate(write_fd, file_length);
        write_addr = (char *)mmap(NULL, file_length, PROT_READ|PROT_WRITE, MAP_SHARED, write_fd, 0);
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
        && req_control->reqType >= 101
        && req_control->reqType <= 109) {
        if (req_control->reqType == CMD_FLOAT_CMP) {
            EncFloatCmpRequestData* req = (EncFloatCmpRequestData*)req_buffer;
            size_t length = sizeof(int) * 3 + ENC_FLOAT4_LENGTH * 2 + sizeof(uint64_t);
            char* dst = get_write_buffer(length);
            memcpy(dst, &req_control->reqType, sizeof(int));
            dst += sizeof(int);
            memcpy(dst, &req->left, ENC_FLOAT4_LENGTH);
            dst += ENC_FLOAT4_LENGTH;
            memcpy(dst, &req->right, ENC_FLOAT4_LENGTH);
            dst += ENC_FLOAT4_LENGTH;
            memcpy(dst, &req->cmp, sizeof(int));
            dst += sizeof(int);
            memcpy(dst, &req_control->resp, sizeof(int));
            dst += sizeof(int);
            uint64_t timestamp = get_timestamp();
            memcpy(dst, &timestamp, sizeof(uint64_t));
        } else if (req_control->reqType == CMD_FLOAT_SUM_BULK) {
            EncFloatBulkRequestData* req = (EncFloatBulkRequestData*)req_buffer;
            size_t length = sizeof(int) * 3 + ENC_FLOAT4_LENGTH + sizeof(uint64_t);
            char* dst = get_write_buffer(length);
            memcpy(dst, &req_control->reqType, sizeof(int));
            dst += sizeof(int);
            memcpy(dst, &req->bulk_size, sizeof(int));
            dst += sizeof(int);
            memcpy(dst, &req->res, ENC_FLOAT4_LENGTH);
            dst += ENC_FLOAT4_LENGTH;
            memcpy(dst, &req_control->resp, sizeof(int));
            dst += sizeof(int);
            uint64_t timestamp = get_timestamp();
            memcpy(dst, &timestamp, sizeof(uint64_t));

            dst = get_write_buffer(req->bulk_size * ENC_FLOAT4_LENGTH);
            for (int i = 0; i < req->bulk_size; i++) {
                memcpy(dst, &req->items[i], ENC_FLOAT4_LENGTH);
                dst += ENC_FLOAT4_LENGTH;
            }
        } else {
            EncFloatCalcRequestData* req = (EncFloatCalcRequestData*)req_buffer;
            size_t length = sizeof(int) * 2 + ENC_FLOAT4_LENGTH * 3 + sizeof(uint64_t);
            char* dst = get_write_buffer(length);
            memcpy(dst, &req->op, sizeof(int));
            dst += sizeof(int);
            memcpy(dst, &req->left, ENC_FLOAT4_LENGTH);
            dst += ENC_FLOAT4_LENGTH;
            memcpy(dst, &req->right, ENC_FLOAT4_LENGTH);
            dst += ENC_FLOAT4_LENGTH;
            memcpy(dst, &req->res, ENC_FLOAT4_LENGTH);
            dst += ENC_FLOAT4_LENGTH;
            memcpy(dst, &req_control->resp, sizeof(int));
            dst += sizeof(int);
            uint64_t timestamp = get_timestamp();
            memcpy(dst, &timestamp, sizeof(uint64_t));
        }

    } // end of float
    else if (req_control->reqType >= 1
        && req_control->reqType <= 10
        && req_control->reqType != CMD_INT_ENC
        && req_control->reqType != CMD_INT_DEC) {
        if (req_control->reqType == CMD_INT_CMP) {
            EncIntCmpRequestData* req = (EncIntCmpRequestData*)req_buffer;
            size_t length = sizeof(int) * 3 + ENC_INT32_LENGTH * 2 + sizeof(uint64_t);
            char* dst = get_write_buffer(length);
            memcpy(dst, &req_control->reqType, sizeof(int));
            dst += sizeof(int);
            memcpy(dst, &req->left, ENC_INT32_LENGTH);
            dst += ENC_INT32_LENGTH;
            memcpy(dst, &req->right, ENC_INT32_LENGTH);
            dst += ENC_INT32_LENGTH;
            memcpy(dst, &req->cmp, sizeof(int));
            dst += sizeof(int);
            memcpy(dst, &req_control->resp, sizeof(int));
            dst += sizeof(int);
            uint64_t timestamp = get_timestamp();
            memcpy(dst, &timestamp, sizeof(uint64_t));
        } else if (req_control->reqType == CMD_INT_SUM_BULK) {
            EncIntBulkRequestData* req = (EncIntBulkRequestData*)req_buffer;
            size_t length = sizeof(int) * 3 + ENC_INT32_LENGTH + sizeof(uint64_t);
            char* dst = get_write_buffer(length);
            memcpy(dst, &req_control->reqType, sizeof(int));
            dst += sizeof(int);
            memcpy(dst, &req->bulk_size, sizeof(int));
            dst += sizeof(int);
            memcpy(dst, &req->res, ENC_INT32_LENGTH);
            dst += ENC_INT32_LENGTH;
            memcpy(dst, &req_control->resp, sizeof(int));
            dst += sizeof(int);
            uint64_t timestamp = get_timestamp();
            memcpy(dst, &timestamp, sizeof(uint64_t));

            dst = get_write_buffer(req->bulk_size * ENC_INT32_LENGTH);
            for (int i = 0; i < req->bulk_size; i++) {
                memcpy(dst, &req->items[i], ENC_INT32_LENGTH);
                dst += ENC_INT32_LENGTH;
            }
        } else {
            EncIntCalcRequestData* req = (EncIntCalcRequestData*)req_buffer;
            size_t length = sizeof(int) * 2 + ENC_INT32_LENGTH * 3 + sizeof(uint64_t);
            char* dst = get_write_buffer(length);
            memcpy(dst, &req->op, sizeof(int));
            dst += sizeof(int);
            memcpy(dst, &req->left, ENC_INT32_LENGTH);
            dst += ENC_INT32_LENGTH;
            memcpy(dst, &req->right, ENC_INT32_LENGTH);
            dst += ENC_INT32_LENGTH;
            memcpy(dst, &req->res, ENC_INT32_LENGTH);
            dst += ENC_INT32_LENGTH;
            memcpy(dst, &req_control->resp, sizeof(int));
            dst += sizeof(int);
            uint64_t timestamp = get_timestamp();
            memcpy(dst, &timestamp, sizeof(uint64_t));
        }
    } else if (req_control->reqType != CMD_STRING_ENC
        && req_control->reqType != CMD_STRING_DEC
        && req_control->reqType >= 201
        && req_control->reqType <= 206) {
        if (req_control->reqType == CMD_STRING_CMP || req_control->reqType == CMD_STRING_LIKE) {
            EncStrCmpRequestData* req = (EncStrCmpRequestData*)req_buffer;
            int left_length = encstr_size(req->left), right_length = encstr_size(req->right);
            int length = sizeof(int) * 5 + left_length + right_length + sizeof(uint64_t);
            char* dst = get_write_buffer(length);
            memcpy(dst, &req_control->reqType, sizeof(int));
            dst += sizeof(int);
            memcpy(dst, &left_length, sizeof(int));
            dst += sizeof(int);
            memcpy(dst, &right_length, sizeof(int));
            dst += sizeof(int);
            memcpy(dst, &req->left, left_length);
            dst += left_length;
            memcpy(dst, &req->right, right_length);
            dst += right_length;
            memcpy(dst, &req->cmp, sizeof(int));
            dst += sizeof(int);
            memcpy(dst, &req_control->resp, sizeof(int));
            dst += sizeof(int);
            uint64_t timestamp = get_timestamp();
            memcpy(dst, &timestamp, sizeof(uint64_t));
        } else if (req_control->reqType == CMD_STRING_SUBSTRING) {
            SubstringRequestData* req = (SubstringRequestData*)req_buffer;
            int str_length = encstr_size(req->str), result_length = encstr_size(req->res);
            size_t length = sizeof(int) * 4 + str_length + result_length + 2 * ENC_INT32_LENGTH + sizeof(uint64_t);
            char* dst = get_write_buffer(length);
            memcpy(dst, &req_control->reqType, sizeof(int));
            dst += sizeof(int);
            memcpy(dst, &str_length, sizeof(int));
            dst += sizeof(int);
            memcpy(dst, &result_length, sizeof(int));
            dst += sizeof(int);
            memcpy(dst, &req->str, str_length);
            dst += str_length;
            memcpy(dst, &req->begin, ENC_INT32_LENGTH);
            dst += ENC_INT32_LENGTH;
            memcpy(dst, &req->end, ENC_INT32_LENGTH);
            dst += ENC_INT32_LENGTH;
            memcpy(dst, &req->res, result_length);
            dst += result_length;
            memcpy(dst, &req_control->resp, sizeof(int));
            dst += sizeof(int);
            uint64_t timestamp = get_timestamp();
            memcpy(dst, &timestamp, sizeof(uint64_t));
        } else {
            EncStrCalcRequestData* req = (EncStrCalcRequestData*)req_buffer;
            int left_length = encstr_size(req->left), right_length = encstr_size(req->right), res_length = encstr_size(req->res);
            int length = sizeof(int) * 5 + left_length + right_length + res_length + sizeof(uint64_t);
            char* dst = get_write_buffer(length);
            memcpy(dst, &req->op, sizeof(int));
            dst += sizeof(int);
            memcpy(dst, &left_length, sizeof(int));
            dst += sizeof(int);
            memcpy(dst, &right_length, sizeof(int));
            dst += sizeof(int);
            memcpy(dst, &res_length, sizeof(int));
            dst += sizeof(int);
            memcpy(dst, &req->left, left_length);
            dst += left_length;
            memcpy(dst, &req->right, right_length);
            dst += right_length;
            memcpy(dst, &req->res, res_length);
            dst += res_length;
            memcpy(dst, &req_control->resp, sizeof(int));
            dst += sizeof(int);
            uint64_t timestamp = get_timestamp();
            memcpy(dst, &timestamp, sizeof(uint64_t));
        }
    } else if (req_control->reqType >= 150
        && req_control->reqType <= 153
        && req_control->reqType != CMD_TIMESTAMP_DEC
        && req_control->reqType != CMD_TIMESTAMP_ENC) {
        if (req_control->reqType == CMD_TIMESTAMP_CMP) {
            EncTimestampCmpRequestData* req = (EncTimestampCmpRequestData*)req_buffer;
            int length = sizeof(int) * 3 + ENC_TIMESTAMP_LENGTH * 2 + sizeof(uint64_t);
            char* dst = get_write_buffer(length);
            memcpy(dst, &req_control->reqType, sizeof(int));
            dst += sizeof(int);
            memcpy(dst, &req->left, ENC_TIMESTAMP_LENGTH);
            dst += ENC_TIMESTAMP_LENGTH;
            memcpy(dst, &req->right, ENC_TIMESTAMP_LENGTH);
            dst += ENC_TIMESTAMP_LENGTH;
            memcpy(dst, &req->cmp, sizeof(int));
            dst += sizeof(int);
            memcpy(dst, &req_control->resp, sizeof(int));
            dst += sizeof(int);
            uint64_t timestamp = get_timestamp();
            memcpy(dst, &timestamp, sizeof(uint64_t));
        } else if (req_control->reqType == CMD_TIMESTAMP_EXTRACT_YEAR) {
            EncTimestampExtractYearRequestData* req = (EncTimestampExtractYearRequestData*)req_buffer;
            size_t length = sizeof(int) * 2 + ENC_TIMESTAMP_LENGTH + ENC_INT32_LENGTH + sizeof(uint64_t);
            char* dst = get_write_buffer(length);
            memcpy(dst, &req_control->reqType, sizeof(int));
            dst += sizeof(int);
            memcpy(dst, &req->in, ENC_TIMESTAMP_LENGTH);
            dst += ENC_TIMESTAMP_LENGTH;
            memcpy(dst, &req->res, ENC_INT32_LENGTH);
            dst += ENC_INT32_LENGTH;
            memcpy(dst, &req_control->resp, sizeof(int));
            dst += sizeof(int);
            uint64_t timestamp = get_timestamp();
            memcpy(dst, &timestamp, sizeof(uint64_t));
        }
    }
}