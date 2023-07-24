#include <enc_int_ops.hpp>
#include <interface.hpp>
#include <request.hpp>

int enc_int_encrypt(int pSrc, EncInt* pDst)
{
    auto req = EncRequest<int, EncInt, CMD_INT_ENC>(&pSrc, pDst);
    TEEInvoker* invoker = TEEInvoker::getInstance();
    int resp = invoker->sendRequest(&req);
    return resp;
}

int enc_int_decrypt(EncInt* pSrc, int* pDst)
{
    auto req = DecRequest<EncInt, int, CMD_INT_DEC>(pSrc, pDst);
    TEEInvoker* invoker = TEEInvoker::getInstance();
    int resp = invoker->sendRequest(&req);
    return resp;
}

////////////////////////////////////////////////////////////////////////////////

static int enc_int_calc(int cmd, EncInt* left, EncInt* right, EncInt* res)
{
    auto req = CalcRequest<EncInt>(cmd, left, right, res);
    TEEInvoker* invoker = TEEInvoker::getInstance();
    int resp = invoker->sendRequest(&req);
    return resp;
}

int enc_int_add(EncInt* left, EncInt* right, EncInt* res)
{
    return enc_int_calc(CMD_INT_PLUS, left, right, res);
}

int enc_int_sub(EncInt* left, EncInt* right, EncInt* res)
{
    int resp = enc_int_calc(CMD_INT_MINUS, left, right, res);
    return resp;
}

int enc_int_mult(EncInt* left, EncInt* right, EncInt* res)
{
    int resp = enc_int_calc(CMD_INT_MULT, left, right, res);
    return resp;
}

int enc_int_div(EncInt* left, EncInt* right, EncInt* res)
{
    int resp = enc_int_calc(CMD_INT_DIV, left, right, res);
    return resp;
}

int enc_int_pow(EncInt* left, EncInt* right, EncInt* res)
{
    int resp = enc_int_calc(CMD_INT_EXP, left, right, res);
    return resp;
}

int enc_int_mod(EncInt* left, EncInt* right, EncInt* res)
{
    int resp = enc_int_calc(CMD_INT_MOD, left, right, res);
    return resp;
}

////////////////////////////////////////////////////////////////////////////////

int enc_int_cmp(EncInt* left, EncInt* right, int* res)
{
    auto req = CmpRequest<EncInt, CMD_INT_CMP>(left, right, res);
    TEEInvoker* invoker = TEEInvoker::getInstance();
    int resp = invoker->sendRequest(&req);
    return resp;
}

////////////////////////////////////////////////////////////////////////////////

int enc_int_sum_bulk(size_t bulk_size, EncInt* arg1, EncInt* res)
{
    auto req = BulkRequest<EncInt>(CMD_INT_SUM_BULK, bulk_size, arg1, res);
    TEEInvoker* invoker = TEEInvoker::getInstance();
    int resp = invoker->sendRequest(&req);
    return resp;
}
