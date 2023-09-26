#include "enc_int_ops.h"
#include "plain_int_ops.h"
#include "base64.h"
#include <string>
#include <fstream>
#include <iostream>
using namespace std;

extern ofstream outfile;

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

    {
        if (0 == req->cmp) outfile << "== ";
        else if (-1 == req->cmp) outfile << "< ";
        else if (1 == req->cmp) outfile << "> ";
        outfile << b64_int(&req->left) << " " << b64_int(&req->right) << " True" << endl;
    }

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

    {
        switch (req->common.reqType)
        {
        case CMD_INT_PLUS:  outfile << "+ "; break;
        case CMD_INT_MINUS: outfile << "- "; break;
        case CMD_INT_MULT:  outfile << "* "; break;
        case CMD_INT_DIV:   outfile << "/ "; break;
        case CMD_INT_MOD:   outfile << "% "; break;
        case CMD_INT_EXP:   outfile << "^ "; break;
        default: break;
        }
        outfile << b64_int(&req->left) << " " << b64_int(&req->right) << " " << b64_int(&req->res) << endl;
    }

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

    {
        outfile << "SUM ";
        for (int id = 0; id < req->bulk_size; id++)
            outfile << b64_int(&array[id]) << " ";
        outfile << b64_int(&req->res) << endl;
    }

    return resp;
}
