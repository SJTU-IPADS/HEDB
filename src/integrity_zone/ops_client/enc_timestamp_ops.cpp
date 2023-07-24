#include <enc_timestamp_ops.hpp>
#include <interface.hpp>
#include <request.hpp>

int enc_timestamp_encrypt(TIMESTAMP* src, EncTimestamp* dst)
{
    auto req = EncRequest<TIMESTAMP, EncTimestamp, CMD_TIMESTAMP_ENC>(src, dst);
    TEEInvoker* invoker = TEEInvoker::getInstance();
    int resp = invoker->sendRequest(&req);
    return resp;
}

int enc_timestamp_decrypt(EncTimestamp* src, TIMESTAMP* dst)
{
    auto req = DecRequest<EncTimestamp, TIMESTAMP, CMD_TIMESTAMP_DEC>(src, dst);
    TEEInvoker* invoker = TEEInvoker::getInstance();
    int resp = invoker->sendRequest(&req);
    return resp;
}

////////////////////////////////////////////////////////////////////////////////

int enc_timestamp_cmp(EncTimestamp* left, EncTimestamp* right, int* res)
{
    auto req = CmpRequest<EncTimestamp, CMD_TIMESTAMP_CMP>(left, right, res);
    TEEInvoker* invoker = TEEInvoker::getInstance();
    int resp = invoker->sendRequest(&req);
    return resp;
}

////////////////////////////////////////////////////////////////////////////////

int enc_timestamp_extract_year(EncTimestamp* in, EncInt* out)
{
    auto req = OneArgRequest<EncTimestamp, EncInt, CMD_TIMESTAMP_EXTRACT_YEAR>(in, out);
    TEEInvoker* invoker = TEEInvoker::getInstance();
    int resp = invoker->sendRequest(&req);
    return resp;
}
