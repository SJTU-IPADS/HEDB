#include <enc_text_ops.hpp>
#include <interface.hpp>
#include <request.hpp>

int enc_text_encrypt(Str* in, EncStr* out)
{
    auto req = EncRequest<Str, EncStr, CMD_STRING_ENC>(in, out);
    TEEInvoker* invoker = TEEInvoker::getInstance();
    int resp = invoker->sendRequest(&req);
    return resp;
}

int enc_text_decrypt(EncStr* in, Str* out)
{
    auto req = DecRequest<EncStr, Str, CMD_STRING_DEC>(in, out);
    TEEInvoker* invoker = TEEInvoker::getInstance();
    int resp = invoker->sendRequest(&req);
    return resp;
}

////////////////////////////////////////////////////////////////////////////////

int enc_text_cmp(EncStr* left, EncStr* right, int* res)
{
    auto req = CmpRequest<EncStr, CMD_STRING_CMP>(left, right, res);
    TEEInvoker* invoker = TEEInvoker::getInstance();
    int resp = invoker->sendRequest(&req);
    return resp;
}

////////////////////////////////////////////////////////////////////////////////

int enc_text_concatenate(EncStr* left, EncStr* right, EncStr* res)
{
    auto req = CalcRequest<EncStr>(CMD_STRING_CONCAT, left, right, res);
    TEEInvoker* invoker = TEEInvoker::getInstance();
    int resp = invoker->sendRequest(&req);
    return resp;
}

int enc_text_substring(EncStr* str, int32_t start, int32_t length, EncStr* res)
{
    auto req = ThreeArgRequest<EncStr, int32_t, int32_t, EncStr, CMD_STRING_SUBSTRING>(str, &start, &length, res);
    TEEInvoker* invoker = TEEInvoker::getInstance();
    int resp = invoker->sendRequest(&req);
    return resp;
}

int enc_text_like(EncStr* str, EncStr* pattern, int* res)
{
    auto req = CmpRequest<EncStr, CMD_STRING_LIKE>(str, pattern, res);
    TEEInvoker* invoker = TEEInvoker::getInstance();
    int resp = invoker->sendRequest(&req);
    return resp;
}
