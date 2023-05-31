
#include <enc_text_ops.h>
#include <like_match.h>
#include <plain_text_ops.h>

int enc_text_cmp(EncStrCmpRequestData* req)
{
    Str left, right;
    int resp = 0;
    bool found;
    //  decrypt left

    left = text_map_find(t_map_p, &req->left, &found);
    if (!found) {
        left.len = req->left.len - IV_SIZE - TAG_SIZE;
        resp = decrypt_bytes_para((uint8_t*)&req->left.enc_cstr, req->left.len, (uint8_t*)&left.data, left.len);
        if (resp != 0)
            return resp;
        text_map_insert(t_map_p, &req->left, &left);
    }

    // decrypt right
    right = text_map_find(t_map_p, &req->right, &found);
    if (!found) {
        right.len = req->right.len - IV_SIZE - TAG_SIZE;
        resp = decrypt_bytes((uint8_t*)&req->right.enc_cstr, req->right.len, (uint8_t*)&right.data, right.len);
        if (resp != 0)
            return resp;
        text_map_insert(t_map_p, &req->right, &right);
    }

    decrypt_wait(NULL, 0);
    left.data[left.len] = '\0';
    right.data[right.len] = '\0';

    req->cmp = plain_text_cmp((char*)left.data, left.len, (char*)right.data, right.len);
    // printf("%d, %s, %s, %d\n",req->common.reqType, left.data, right.data, req->cmp);

    return resp;
}
int enc_text_like(EncStrLikeRequestData* req)
{

    Str left, right;
    int resp = 0;
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

    // printf("%d, %s, %s, %d\n",req->common.reqType, left.data, right.data,req->cmp);

    return resp;
}
int enc_text_concatenate(EncStrCalcRequestData* req)
{

    Str left, right;
    int resp = 0;
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
    // printf("%d, %s, %s, ",req->common.reqType, left.data,right.data);

    plain_text_concat((char*)left.data, &left.len, (char*)right.data, right.len);

    // printf("%s\n",req->common.reqType, left.data);

    req->res.len = left.len + IV_SIZE + TAG_SIZE;
    resp = encrypt_bytes((uint8_t*)&left.data, left.len, (uint8_t*)&req->res.enc_cstr, req->res.len);

    return resp;
}
int enc_text_substring(SubstringRequestData* req)
{

    Str str, sub;
    int begin, end;
    int resp = 0;

    str.len = req->str.len - IV_SIZE - TAG_SIZE;
    resp = decrypt_bytes((uint8_t*)&req->str.enc_cstr, req->str.len, (uint8_t*)&str.data, str.len);
    if (resp != 0)
        return resp;
    str.data[str.len] = '\0';

    resp = decrypt_bytes((uint8_t*)&req->begin, sizeof(req->begin), (uint8_t*)&begin, sizeof(begin));
    if (resp != 0)
        return resp;

    resp = decrypt_bytes((uint8_t*)&req->end, sizeof(req->end), (uint8_t*)&end, sizeof(end));
    if (resp != 0)
        return resp;

    plain_text_substring(str.data, begin, end, sub.data);
    sub.len = end - begin + 1;

    req->res.len = sub.len + IV_SIZE + TAG_SIZE;
    resp = encrypt_bytes((uint8_t*)&sub.data, sub.len, (uint8_t*)&req->res.enc_cstr, req->res.len);

    return resp;
}
