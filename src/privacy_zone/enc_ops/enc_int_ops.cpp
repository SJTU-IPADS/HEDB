// SPDX-License-Identifier: Mulan PSL v2
/*
 * Copyright (c) 2021 - 2023 The HEDB Project.
 */

#include "enc_int_ops.h"
#include "plain_int_ops.h"
#include "base64.h"
#include <string>
#include <fstream>
#include <iostream>
using namespace std;

// #define DEBUG_MODE

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

#ifdef DEBUG_MODE
    {
        if (0 == req->cmp) cout << "== ";
        else if (-1 == req->cmp) cout << "< ";
        else if (1 == req->cmp) cout << "> ";
        cout << b64_int(&req->left) << " " << b64_int(&req->right) << " True" << endl;
    }
#endif
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

#ifdef DEBUG_MODE
    {
        switch (req->common.reqType)
        {
        case CMD_INT_PLUS:  cout << "+ "; break;
        case CMD_INT_MINUS: cout << "- "; break;
        case CMD_INT_MULT:  cout << "* "; break;
        case CMD_INT_DIV:   cout << "/ "; break;
        case CMD_INT_MOD:   cout << "% "; break;
        case CMD_INT_EXP:   cout << "^ "; break;
        default: break;
        }
        cout << b64_int(&req->left) << " " << b64_int(&req->right) << " " << b64_int(&req->res) << endl;
    }
#endif
    return resp;
}

int enc_int32_sum_bulk(EncIntBulkRequestData* req)
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

    int res = plain_int32_sum_bulk(req->bulk_size, plain_array);

    resp = encrypt_bytes((uint8_t*)&res, sizeof(res), (uint8_t*)&req->res, sizeof(req->res));

#ifdef DEBUG_MODE
    {
        cout << "SUM ";
        for (int id = 0; id < req->bulk_size; id++)
            cout << b64_int(&array[id]) << " ";
        cout << b64_int(&req->res) << endl;
    }
#endif
    return resp;
}
