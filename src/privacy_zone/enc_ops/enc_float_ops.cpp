#include "enc_float_ops.h"
#include "plain_float_ops.h"
#include <time.h>
#include "base64.h"
#include <string>
using namespace std;

static string b64_float(EncFloat* in)
{
    char b64_float4[ENC_FLOAT_B64_LENGTH + 1] = { 0 };
    toBase64((const unsigned char*)in, sizeof(EncFloat), b64_float4);
    return b64_float4;
}

int enc_float32_cmp(EncFloatCmpRequestData* req)
{
    float left, right;
    int resp = 0;

    resp = decrypt_bytes_para((uint8_t*)&req->left, sizeof(req->left), (uint8_t*)&left, sizeof(float));

    if (resp != 0)
        return resp;

    resp = decrypt_bytes((uint8_t*)&req->right, sizeof(req->right), (uint8_t*)&right, sizeof(float));
    if (resp != 0)
        return resp;

    decrypt_wait((uint8_t*)&left, sizeof(left));

    req->cmp = plain_float_cmp(left, right);
    printf("[LOG Admin] <%d> %s %s => %d\n", req->common.reqType,
        b64_float(&req->left).c_str(), b64_float(&req->right).c_str(), req->cmp);
    printf("[LOG Client] <%d> %f %f => %d\n", req->common.reqType, left, right, req->cmp);

    return resp;
}

/* comment from PSQL code
 * There isn't any way to check for underflow of addition/subtraction
 * because numbers near the underflow value have already been rounded to
 * the point where we can't detect that the two values were originally
 * different, e.g. on x86, '1e-45'::float4 == '2e-45'::float4 ==
 * 1.4013e-45.
 * we have only 4 bytes for float4 datatype
 * we can check if the out size is less 8^4
 *
 */
int enc_float32_calc(EncFloatCalcRequestData* req)
{
    float left, right, res;
    int resp = 0;
    bool para_issued = false;

    resp = decrypt_bytes_para((uint8_t*)&req->left, sizeof(req->left), (uint8_t*)&left, sizeof(left));
    para_issued = true;
    if (resp != 0)
        return resp;

    resp = decrypt_bytes((uint8_t*)&req->right, sizeof(req->right), (uint8_t*)&right, sizeof(right));
    if (resp != 0)
        return resp;
    if (para_issued)
        decrypt_wait((uint8_t*)&left, sizeof(left));

    res = plain_float_calc(req->common.reqType, left, right);

    resp = encrypt_bytes((uint8_t*)&res, sizeof(res), (uint8_t*)&req->res, sizeof(req->res));

    printf("[LOG Admin] <%d> %s %s => %s\n", req->common.reqType,
        b64_float(&req->left).c_str(), b64_float(&req->right).c_str(), b64_float(&req->res).c_str());
    printf("[LOG Client] <%d> %f %f => %f\n", req->common.reqType, left, right, res);

    return resp;
}

static int bulk_count = 0;
int enc_float32_bulk(EncFloatBulkRequestData* req)
{
    bulk_count++;

    int bulk_size = req->bulk_size;
    EncFloat* array = req->items;
    float values[bulk_size];
    int count = 0, resp = 0;
    bool parallel_issued = false;
    while (count < bulk_size) {
        if (MAX_DECRYPT_THREAD >= 15) {
            resp = decrypt_bytes_para((uint8_t*)&array[count], sizeof(EncFloat), (uint8_t*)&values[count], sizeof(float));
        } else {
            resp = decrypt_bytes((uint8_t*)&array[count], sizeof(EncFloat), (uint8_t*)&values[count], sizeof(float));
        }
        parallel_issued = true;
        if (resp != 0)
            return resp;
        count++;
    }

    if (parallel_issued) {
        decrypt_wait(NULL, 0);
    }

    float res = (float)plain_float_bulk(req->common.reqType, bulk_size, values);

    resp = encrypt_bytes((uint8_t*)&res, sizeof(float), (uint8_t*)&req->res, sizeof(req->res));

    printf("[LOG Admin] <%d> ", req->common.reqType);
    for (int id = 0; id < req->bulk_size; id++)
        printf("%s ", b64_float(&array[id]).c_str());
    printf("=> %s\n", b64_float(&req->res).c_str());
    printf("[LOG Client] <%d> ", req->common.reqType);
    for (int id = 0; id < req->bulk_size; id++)
        printf("%f ", values[id]);
    printf("=> %f\n", res);

    return resp;
}

int enc_float32_eval_expr(EncFloatEvalExprRequestData* req)
{
    int arg_cnt = req->arg_cnt, resp = 0, i = 0;
    Str expr = req->expr;
    EncFloat* enc_arr = req->items;
    float res = 0;
    float arr[EXPR_MAX_SIZE];
    bool parallel_issued = false;
    for (i = 0; i < arg_cnt - 1; ++i) {
        resp = decrypt_bytes_para((uint8_t*)&enc_arr[i], sizeof(EncFloat), (uint8_t*)&arr[i], sizeof(float));
        parallel_issued = true;
        if (resp != 0) {
            goto error;
        }
    }

    resp = decrypt_bytes((uint8_t*)&enc_arr[i], sizeof(EncFloat), (uint8_t*)&arr[i], sizeof(float));
    if (resp != 0) {
        goto error;
    }

    if (parallel_issued) {
        decrypt_wait(NULL, 0);
    }

    res = plain_float_eval_expr(expr.data, arr);
    resp = encrypt_bytes((uint8_t*)&res, sizeof(float), (uint8_t*)&req->res, sizeof(req->res));
    return resp;

error:

    if (parallel_issued) {
        decrypt_wait(NULL, 0);
    }

    return resp;
}
