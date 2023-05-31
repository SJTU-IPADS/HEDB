//
// Created by xuyang on 2022/1/8.
//
#include <enc_float_ops.hpp>
#include <interface.hpp>
#include <request.hpp>

int enc_float_cmp(EncFloat* left, EncFloat* right, int* res)
{
    auto req = CmpRequest<EncFloat, CMD_FLOAT_CMP>(left, right, res);
    TEEInvoker* invoker = TEEInvoker::getInstance();
    int resp = invoker->sendRequest(&req);
    return resp;
}

int enc_float_encrypt(float in, EncFloat* out)
{
    auto req = EncRequest<float, EncFloat, CMD_FLOAT_ENC>(&in, out);
    TEEInvoker* invoker = TEEInvoker::getInstance();
    int resp = invoker->sendRequest(&req);
    return resp;
}

int enc_float_decrypt(EncFloat* in, float* out)
{
    auto req = DecRequest<EncFloat, float, CMD_FLOAT_DEC>(in, out);
    TEEInvoker* invoker = TEEInvoker::getInstance();
    int resp = invoker->sendRequest(&req);
    return resp;
}

int enc_float_calc(int cmd, EncFloat* left, EncFloat* right, EncFloat* res)
{
    auto req = CalcRequest<EncFloat>(cmd, left, right, res);
    TEEInvoker* invoker = TEEInvoker::getInstance();
    int resp = invoker->sendRequest(&req);
    return resp;
}

int enc_float_add(EncFloat* left, EncFloat* right, EncFloat* res)
{
    return enc_float_calc(CMD_FLOAT_PLUS, left, right, res);
}

int enc_float_sub(EncFloat* left, EncFloat* right, EncFloat* res)
{
    return enc_float_calc(CMD_FLOAT_MINUS, left, right, res);
}

int enc_float_mult(EncFloat* left, EncFloat* right, EncFloat* res)
{
    return enc_float_calc(CMD_FLOAT_MULT, left, right, res);
}

int enc_float_div(EncFloat* left, EncFloat* right, EncFloat* res)
{
    return enc_float_calc(CMD_FLOAT_DIV, left, right, res);
}

int enc_float_pow(EncFloat* left, EncFloat* right, EncFloat* res)
{
    return enc_float_calc(CMD_FLOAT_EXP, left, right, res);
}

int enc_float_mod(EncFloat* left, EncFloat* right, EncFloat* res)
{
    return enc_float_calc(CMD_FLOAT_MOD, left, right, res);
}

int enc_float_sum_bulk(size_t bulk_size, EncFloat* bulk_data, EncFloat* res)
{
    auto req = BulkRequest<EncFloat>(CMD_FLOAT_SUM_BULK, bulk_size, bulk_data, res);
    TEEInvoker* invoker = TEEInvoker::getInstance();
    int resp = invoker->sendRequest(&req);
    return resp;
}

int enc_float_eval_expr(size_t arg_cnt, Str expr, EncFloat** arg_data, EncFloat* res)
{
    auto req = EvalExprRequest<EncFloat>(CMD_FLOAT_EVAL_EXPR, arg_cnt, expr, arg_data, res);
    TEEInvoker* invoker = TEEInvoker::getInstance();
    int resp = invoker->sendRequest(&req);
    return resp;
}
