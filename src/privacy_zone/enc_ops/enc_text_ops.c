#include "enc_text_ops.h"
#include <like_match.h>
#include "plain_text_ops.h"

int enc_text_cmp(EncStrCmpRequestData* req)
{
    int resp = 0;
    Str left, right;

    left.len = req->left.len - IV_SIZE - TAG_SIZE;
    resp = decrypt_bytes_para((uint8_t*)&req->left.enc_cstr, req->left.len, (uint8_t*)&left.data, left.len);
    if (resp != 0)
        return resp;

    right.len = req->right.len - IV_SIZE - TAG_SIZE;
    resp = decrypt_bytes((uint8_t*)&req->right.enc_cstr, req->right.len, (uint8_t*)&right.data, right.len);
    if (resp != 0)
        return resp;

    decrypt_wait(NULL, 0);
    left.data[left.len] = '\0';
    right.data[right.len] = '\0';

    req->cmp = plain_text_cmp((char*)left.data, left.len, (char*)right.data, right.len);
    return resp;
}

int enc_text_like(EncStrLikeRequestData* req)
{
    int resp = 0;
    Str left, right;

    left.len = req->left.len - IV_SIZE - TAG_SIZE;
    resp = decrypt_bytes_para((uint8_t*)&req->left.enc_cstr, req->left.len, (uint8_t*)&left.data, left.len);
    if (resp != 0)
        return resp;
    left.data[left.len] = '\0';

    right.len = req->right.len - IV_SIZE - TAG_SIZE;
    resp = decrypt_bytes((uint8_t*)&req->right.enc_cstr, req->right.len, (uint8_t*)&right.data, right.len);
    if (resp != 0)
        return resp;
    right.data[right.len] = '\0';
    decrypt_wait(NULL, 0);

    req->cmp = plain_text_like((char*)left.data, left.len, (char*)right.data, right.len);
    return resp;
}

int enc_text_concatenate(EncStrCalcRequestData* req)
{
    int resp = 0;
    Str left, right;

    left.len = req->left.len - IV_SIZE - TAG_SIZE;
    resp = decrypt_bytes((uint8_t*)&req->left.enc_cstr, req->left.len, (uint8_t*)&left.data, left.len);
    if (resp != 0)
        return resp;
    left.data[left.len] = '\0';

    right.len = req->right.len - IV_SIZE - TAG_SIZE;
    resp = decrypt_bytes((uint8_t*)&req->right.enc_cstr, req->right.len, (uint8_t*)&right.data, right.len);
    if (resp != 0)
        return resp;
    right.data[right.len] = '\0';

    plain_text_concat((char*)left.data, &left.len, (char*)right.data, right.len);
    req->res.len = left.len + IV_SIZE + TAG_SIZE;
    resp = encrypt_bytes((uint8_t*)&left.data, left.len, (uint8_t*)&req->res.enc_cstr, req->res.len);
    return resp;
}
int enc_text_substring(SubstringRequestData* req)
{
    int resp = 0;
    Str str, sub;
    int32_t start, length;

    str.len = req->str.len - IV_SIZE - TAG_SIZE;
    resp = decrypt_bytes((uint8_t*)&req->str.enc_cstr, req->str.len, (uint8_t*)&str.data, str.len);
    if (resp != 0)
        return resp;
    str.data[str.len] = '\0';

    memcpy(&start, (uint8_t*)&req->start, sizeof(start));
    memcpy(&length, (uint8_t*)&req->length, sizeof(length));
    // resp = decrypt_bytes((uint8_t*)&req->start, sizeof(req->start), (uint8_t*)&start, sizeof(start));
    // resp = decrypt_bytes((uint8_t*)&req->length, sizeof(req->length), (uint8_t*)&length, sizeof(length));

    plain_text_substring(str.data, start, length, sub.data);
    sub.len = length;

    req->res.len = sub.len + IV_SIZE + TAG_SIZE;
    resp = encrypt_bytes((uint8_t*)&sub.data, sub.len, (uint8_t*)&req->res.enc_cstr, req->res.len);
    return resp;
}
