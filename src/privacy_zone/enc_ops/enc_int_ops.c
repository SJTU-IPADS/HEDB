#include "enc_int_ops.h"
#include "plain_int_ops.h"

int enc_int32_calc(EncIntCalcRequestData* req)
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

    int res = plain_int32_calc(req->common.reqType, left, right);
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

    int res = plain_int32_bulk(req->common.reqType, req->bulk_size, plain_array);
    resp = encrypt_bytes((uint8_t*)&res, sizeof(res), (uint8_t*)&req->res, sizeof(req->res));
    return resp;
}
