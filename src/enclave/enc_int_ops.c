#include "enc_int_ops.h"
#include "plain_int_ops.h"
// extern int decrypt_status;

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

int enc_int32_calc(EncIntCalcRequestData* req)
{

    int left, right;
    int resp = 0;
    // printf("call add\n");
    resp = decrypt_bytes_para((uint8_t*)&req->left, sizeof(req->left), (uint8_t*)&left, sizeof(left));

    if (resp != 0)
        return resp;
    // printf("req sent\n");

    resp = decrypt_bytes((uint8_t*)&req->right, sizeof(req->right), (uint8_t*)&right, sizeof(right));
    if (resp != 0)
        return resp;

    decrypt_wait((uint8_t*)&left, sizeof(left));

    // printf("clac type %d, %f, %f, ", req->common.reqType, left, right);
    int res = plain_int32_calc(req->common.reqType, left, right);
    // printf("%d, %d, %d, %d\n",req->common.reqType, left, right, res);
    resp = encrypt_bytes((uint8_t*)&res, sizeof(res), (uint8_t*)&req->res, sizeof(req->res));
    return resp;
}

int enc_int32_cmp(EncIntCmpRequestData* req)
{
    int left, right;
    int resp = 0;
    resp = decrypt_bytes_para((uint8_t*)&req->left, sizeof(req->left), (uint8_t*)&left, sizeof(left));
    if (resp != 0)
        return resp;

    resp = decrypt_bytes((uint8_t*)&req->right, sizeof(req->right), (uint8_t*)&right, sizeof(right));
    if (resp != 0)
        return resp;
    decrypt_wait((uint8_t*)&left, sizeof(left));

    req->cmp = plain_int32_cmp(left, right);
    // printf("%d, %d, %d, %d\n",req->common.reqType, left, right, req->cmp);

    return resp;
}

int enc_int32_bulk(EncIntBulkRequestData* req)
{
    int bulk_size = req->bulk_size;
    EncInt* array = req->items;
    int count = 0, resp = 0;
    int plain_array[BULK_SIZE];
    while (count < bulk_size) {
        resp = decrypt_bytes((uint8_t*)&array[count], sizeof(EncInt), (uint8_t*)&plain_array[count], sizeof(int));
        if (resp != 0)
            return resp;
        count++;
    }
    int res = plain_int32_bulk(req->common.reqType,
        req->bulk_size, plain_array);

    resp = encrypt_bytes((uint8_t*)&res, sizeof(res), (uint8_t*)&req->res, sizeof(req->res));
    return resp;
}
