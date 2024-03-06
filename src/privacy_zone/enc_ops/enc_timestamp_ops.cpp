// SPDX-License-Identifier: Mulan PSL v2
/*
 * Copyright (c) 2021 - 2023 The HEDB Project.
 */

#include "enc_timestamp_ops.h"
#include "plain_timestamp_ops.h"
#include "base64.h"
#include <string>
using namespace std;

static string b64_timestamp(EncTimestamp* in)
{
    char b64_timestamp[ENC_TIMESTAMP_B64_LENGTH + 1] = { 0 };
    toBase64((const unsigned char*)in, sizeof(EncTimestamp), b64_timestamp);
    return b64_timestamp;
}

int enc_timestamp_cmp(EncTimestampCmpRequestData* req)
{
    int resp = 0;

    TIMESTAMP left, right;

    resp = decrypt_bytes((uint8_t*)&req->left, sizeof(req->left), (uint8_t*)&left, sizeof(left));
    if (resp != 0)
        return resp;

    resp = decrypt_bytes((uint8_t*)&req->right, sizeof(req->right), (uint8_t*)&right, sizeof(right));
    if (resp != 0)
        return resp;

    req->cmp = plain_timestamp_cmp(left, right);
    // printf("[LOG Admin] <%d> %s %s => %d\n", req->common.reqType,
    //     b64_timestamp(&req->left).c_str(), b64_timestamp(&req->right).c_str(), req->cmp);
    // printf("[LOG Client] <%d> %ld %ld => %d\n", req->common.reqType, left, right, req->cmp);

    return resp;
}

int enc_timestamp_extract_year(EncTimestampExtractYearRequestData* req)
{
    int resp = 0;
    TIMESTAMP t;
    int year;
    resp = decrypt_bytes((uint8_t*)&req->in, sizeof(req->in), (uint8_t*)&t, sizeof(t));
    if (resp != 0)
        return resp;
    year = plain_timestamp_extract_year(t);

    resp = encrypt_bytes((uint8_t*)&year, sizeof(year), (uint8_t*)&req->res, sizeof(req->res));
    return resp;
}
