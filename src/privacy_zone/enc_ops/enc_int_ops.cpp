#include "enc_int_ops.h"
#include "plain_int_ops.h"
#include "base64.h"
#include <string>
using namespace std;

static string b64_int(EncInt* in)
{
    char b64_int4[ENC_INT_B64_LENGTH + 1] = { 0 };
    toBase64((const unsigned char*)in, sizeof(EncInt), b64_int4);
    return b64_int4;
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

    printf("[LOG Admin] <%d> %s %s => %d\n", req->common.reqType,
        b64_int(&req->left).c_str(), b64_int(&req->right).c_str(), req->cmp);
    printf("[LOG Client] <%d> %d %d => %d\n", req->common.reqType, left, right, req->cmp);

    return resp;
}

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

    printf("[LOG Admin] <%d> %s %s => %s\n", req->common.reqType,
        b64_int(&req->left).c_str(), b64_int(&req->right).c_str(), b64_int(&req->res).c_str());
    printf("[LOG Client] <%d> %d %d => %d\n", req->common.reqType, left, right, res);

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

    printf("[LOG Admin] <%d> ", req->common.reqType);
    for (int id = 0; id < req->bulk_size; id++)
        printf("%s ", b64_int(&array[id]).c_str());
    printf("=> %s\n", b64_int(&req->res).c_str());
    printf("[LOG Client] <%d> ", req->common.reqType);
    for (int id = 0; id < req->bulk_size; id++)
        printf("%d ", plain_array[id]);
    printf("=> %d\n", res);

    return resp;
}
