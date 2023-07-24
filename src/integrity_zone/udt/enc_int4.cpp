#include "base64.h"
#include <enc_float_ops.hpp>
#include <enc_int_ops.hpp>
#include <extension.hpp>

extern bool clientMode;

#ifdef __cplusplus
extern "C" {
#endif
PG_FUNCTION_INFO_V1(enc_int4_encrypt);
PG_FUNCTION_INFO_V1(enc_int4_decrypt);
PG_FUNCTION_INFO_V1(enc_int4_in);
PG_FUNCTION_INFO_V1(enc_int4_out);
PG_FUNCTION_INFO_V1(enc_int4_add);
PG_FUNCTION_INFO_V1(enc_int4_sub);
PG_FUNCTION_INFO_V1(enc_int4_mult);
PG_FUNCTION_INFO_V1(enc_int4_div);
PG_FUNCTION_INFO_V1(enc_int4_pow);
PG_FUNCTION_INFO_V1(enc_int4_mod);
PG_FUNCTION_INFO_V1(enc_int4_cmp);
PG_FUNCTION_INFO_V1(enc_int4_eq);
PG_FUNCTION_INFO_V1(enc_int4_ne);
PG_FUNCTION_INFO_V1(enc_int4_lt);
PG_FUNCTION_INFO_V1(enc_int4_le);
PG_FUNCTION_INFO_V1(enc_int4_gt);
PG_FUNCTION_INFO_V1(enc_int4_ge);
PG_FUNCTION_INFO_V1(enc_int4_max);
PG_FUNCTION_INFO_V1(enc_int4_min);
PG_FUNCTION_INFO_V1(enc_int4_sum_bulk);
PG_FUNCTION_INFO_V1(enc_int4_avg_bulk);
PG_FUNCTION_INFO_V1(enc_int4_min_bulk);
PG_FUNCTION_INFO_V1(enc_int4_max_bulk);
// PG_FUNCTION_INFO_V1(int4_to_enc_int4);
// PG_FUNCTION_INFO_V1(int8_to_enc_int4);
#ifdef __cplusplus
}
#endif

Datum enc_int4_encrypt(PG_FUNCTION_ARGS)
{
    EncInt* out = (EncInt*)palloc0(sizeof(EncInt));
    int in = PG_GETARG_INT32(0);
    enc_int_encrypt(in, out);
    // ereport(LOG, (errmsg("function encrypt, output: %s", ans)));
    PG_RETURN_CSTRING(out);
}

Datum enc_int4_decrypt(PG_FUNCTION_ARGS)
{
    int ans = 0;
    EncInt* in = PG_GETARG_ENCINT(0);
    enc_int_decrypt(in, &ans);
    // ereport(LOG, (errmsg("function decrypt, output: %d", ans)));
    PG_RETURN_INT32(ans);
}

Datum enc_int4_in(PG_FUNCTION_ARGS)
{
    char* pIn = PG_GETARG_CSTRING(0);
    EncInt* result = (EncInt*)palloc0(sizeof(EncInt));

    if (clientMode == true) { // from plain int4 to cipher int4
        int in = atoi(pIn);
        enc_int_encrypt(in, result);
    } else { // base64 decode to cipher int4
        fromBase64(pIn, strlen(pIn), (unsigned char*)result);
    }
    PG_RETURN_POINTER(result);
}

Datum enc_int4_out(PG_FUNCTION_ARGS)
{
    EncInt* in = PG_GETARG_ENCINT(0);

    if (clientMode == true) { // from cipher int4 to plain int4
        int out;
        char* str = (char*)palloc0(sizeof(EncInt)); // this length is not really meaningful

        enc_int_decrypt(in, &out);
        sprintf(str, "%d", out);
        // ereport(INFO, (errmsg("auto decryption('%p') = %d", in, out)));
        PG_RETURN_CSTRING(str);
    } else { // base64 encode of cipher int4
#define ENC_INT_B64_LENGTH 45 // ((4 * n / 3) + 3) & ~3
        char base64_int4[ENC_INT_B64_LENGTH + 1] = { 0 };

        toBase64((const unsigned char*)in, sizeof(EncInt), base64_int4);
        // ereport(INFO, (errmsg("base64('%p') = %s", in, base64_int4)));
        PG_RETURN_CSTRING(base64_int4);
    }
}

Datum enc_int4_add(PG_FUNCTION_ARGS)
{
    EncInt* left = PG_GETARG_ENCINT(0);
    EncInt* right = PG_GETARG_ENCINT(1);
    EncInt* result = (EncInt*)palloc0(sizeof(EncInt));

    enc_int_add(left, right, result);

    PG_RETURN_CSTRING(result);
}

Datum enc_int4_sub(PG_FUNCTION_ARGS)
{
    EncInt* left = PG_GETARG_ENCINT(0);
    EncInt* right = PG_GETARG_ENCINT(1);
    EncInt* result = (EncInt*)palloc0(sizeof(EncInt));

    enc_int_sub(left, right, result);

    PG_RETURN_CSTRING(result);
}

Datum enc_int4_mult(PG_FUNCTION_ARGS)
{
    EncInt* left = PG_GETARG_ENCINT(0);
    EncInt* right = PG_GETARG_ENCINT(1);
    EncInt* result = (EncInt*)palloc0(sizeof(EncInt));

    enc_int_mult(left, right, result);

    PG_RETURN_CSTRING(result);
}

Datum enc_int4_div(PG_FUNCTION_ARGS)
{
    EncInt* left = PG_GETARG_ENCINT(0);
    EncInt* right = PG_GETARG_ENCINT(1);
    EncInt* result = (EncInt*)palloc0(sizeof(EncInt));

    enc_int_div(left, right, result);

    PG_RETURN_CSTRING(result);
}

Datum enc_int4_pow(PG_FUNCTION_ARGS)
{
    EncInt* left = PG_GETARG_ENCINT(0);
    EncInt* right = PG_GETARG_ENCINT(1);
    EncInt* result = (EncInt*)palloc0(sizeof(EncInt));

    enc_int_pow(left, right, result);

    PG_RETURN_CSTRING(result);
}

Datum enc_int4_mod(PG_FUNCTION_ARGS)
{
    EncInt* left = PG_GETARG_ENCINT(0);
    EncInt* right = PG_GETARG_ENCINT(1);
    EncInt* result = (EncInt*)palloc0(sizeof(EncInt));

    enc_int_mod(left, right, result);

    PG_RETURN_CSTRING(result);
}

Datum enc_int4_cmp(PG_FUNCTION_ARGS)
{
    EncInt* left = PG_GETARG_ENCINT(0);
    EncInt* right = PG_GETARG_ENCINT(1);
    int res;

    enc_int_cmp(left, right, &res);

    PG_RETURN_INT32(res);
}

Datum enc_int4_eq(PG_FUNCTION_ARGS)
{
    EncInt* left = PG_GETARG_ENCINT(0);
    EncInt* right = PG_GETARG_ENCINT(1);
    int res;
    bool cmp = false;

    enc_int_cmp(left, right, &res);

    if (res == 0)
        cmp = true;
    else
        cmp = false;

    PG_RETURN_BOOL(cmp);
}

Datum enc_int4_ne(PG_FUNCTION_ARGS)
{
    EncInt* left = PG_GETARG_ENCINT(0);
    EncInt* right = PG_GETARG_ENCINT(1);
    int res;
    bool cmp = false;

    enc_int_cmp(left, right, &res);

    if (res == 0)
        cmp = false;
    else
        cmp = true;

    PG_RETURN_BOOL(cmp);
}

Datum enc_int4_lt(PG_FUNCTION_ARGS)
{
    EncInt* left = PG_GETARG_ENCINT(0);
    EncInt* right = PG_GETARG_ENCINT(1);
    int res;
    bool cmp = false;

    enc_int_cmp(left, right, &res);

    if (res == -1)
        cmp = true;
    else
        cmp = false;

    PG_RETURN_BOOL(cmp);
}

Datum enc_int4_le(PG_FUNCTION_ARGS)
{
    EncInt* left = PG_GETARG_ENCINT(0);
    EncInt* right = PG_GETARG_ENCINT(1);
    int res;
    bool cmp = false;

    enc_int_cmp(left, right, &res);

    if ((res == -1) || (res == 0))
        cmp = true;
    else
        cmp = false;

    PG_RETURN_BOOL(cmp);
}

Datum enc_int4_gt(PG_FUNCTION_ARGS)
{
    EncInt* left = PG_GETARG_ENCINT(0);
    EncInt* right = PG_GETARG_ENCINT(1);
    int res;
    bool cmp = false;

    enc_int_cmp(left, right, &res);

    if (res == 1)
        cmp = true;
    else
        cmp = false;

    PG_RETURN_BOOL(cmp);
}

Datum enc_int4_ge(PG_FUNCTION_ARGS)
{
    EncInt* left = PG_GETARG_ENCINT(0);
    EncInt* right = PG_GETARG_ENCINT(1);
    int res;
    bool cmp = false;

    enc_int_cmp(left, right, &res);

    if (res == 0 || res == 1)
        cmp = true;
    else
        cmp = false;
    PG_RETURN_BOOL(cmp);
}

/*
 * The function
 * @input: two enc_int4 values
 * @return: the larger enc_int4 value.
 */
Datum enc_int4_max(PG_FUNCTION_ARGS)
{
    EncInt* left = PG_GETARG_ENCINT(0);
    EncInt* right = PG_GETARG_ENCINT(1);
    int res;

    enc_int_cmp(left, right, &res);

    if (res == 1) {
        PG_RETURN_POINTER(left);
    } else {
        PG_RETURN_POINTER(right);
    }
}

/*
 * The function
 * @input: two enc_int4 values
 * @return: the smaller enc_int4 value.
 */
Datum enc_int4_min(PG_FUNCTION_ARGS)
{
    EncInt* left = PG_GETARG_ENCINT(0);
    EncInt* right = PG_GETARG_ENCINT(1);
    int res;

    enc_int_cmp(left, right, &res);

    if (res == 1) {
        PG_RETURN_POINTER(right);
    } else {
        PG_RETURN_POINTER(left);
    }
}

Datum enc_int4_sum_bulk(PG_FUNCTION_ARGS)
{
    ArrayType* v = PG_GETARG_ARRAYTYPE_P(0);
    bool isnull;
    Datum value;

    EncInt* sum = (EncInt*)palloc0(sizeof(EncInt));
    EncInt array[BULK_SIZE];
    int counter = 1;

    // TODO: two copies happens here, for array of encint.
    ArrayMetaState* my_extra = (ArrayMetaState*)fcinfo->flinfo->fn_extra;
    ArrayIterator array_iterator = array_create_iterator(v, 0, my_extra);

    array_iterate(array_iterator, &value, &isnull);
    *sum = *DatumGetEncInt(value);
    array[0] = *sum;
    while (array_iterate(array_iterator, &value, &isnull)) {
        array[counter] = *DatumGetEncInt(value);
        counter++;
        if (counter == BULK_SIZE) {
            enc_int_sum_bulk(BULK_SIZE, array, sum);
            array[0] = *sum;
            counter = 1;
        }
    }
    if (counter > 1) {
        enc_int_sum_bulk(counter, array, sum);
    }

    PG_RETURN_CSTRING(sum);
}

Datum enc_int4_avg_bulk(PG_FUNCTION_ARGS)
{
    ArrayType* v = PG_GETARG_ARRAYTYPE_P(0);
    bool isnull;
    Datum value;
    // int ndims1 = ARR_NDIM(v); // array dimension
    // int* dims1 = ARR_DIMS(v);
    // int nitems = ArrayGetNItems(ndims1, dims1); // number of items in array

    EncInt sum;
    EncInt* res = (EncInt*)palloc0(sizeof(EncInt));
    EncInt array[BULK_SIZE];
    EncInt num;
    EncInt unit; // cipher of '1'
    EncInt num_array[BULK_SIZE]; // nitems of '1'
    int counter; // sum will be at array[0]

    ArrayMetaState* my_extra = (ArrayMetaState*)fcinfo->flinfo->fn_extra;
    ArrayIterator array_iterator = array_create_iterator(v, 0, my_extra);

    array_iterate(array_iterator, &value, &isnull);
    sum = *DatumGetEncInt(value);
    array[0] = sum;
    counter = 1;
    enc_int_div(&array[0], &array[0], &unit); // get the cipher of '1'
    for (int i = 0; i < BULK_SIZE; ++i) { // get nitems of '1'
        num_array[i] = unit;
    }
    while (array_iterate(array_iterator, &value, &isnull)) {
        array[counter] = *DatumGetEncInt(value);
        num_array[counter] = unit;
        counter++;
        if (counter == BULK_SIZE) {
            enc_int_sum_bulk(BULK_SIZE, array, &sum);
            enc_int_sum_bulk(BULK_SIZE, num_array, &num);
            array[0] = sum;
            num_array[0] = num;
            counter = 1;
        }
    }

    if (counter > 1) {
        enc_int_sum_bulk(counter, array, &sum);
        enc_int_sum_bulk(counter, num_array, &num);
    }
    // enc_int_encrypt(nitems, &num);
    enc_int_div(&sum, &num, res);

    PG_RETURN_CSTRING(res);
}

Datum enc_int4_min_bulk(PG_FUNCTION_ARGS)
{
    ArrayType* v = PG_GETARG_ARRAYTYPE_P(0);
    int ans = 0;
    ArrayIterator array_iterator;
    ArrayMetaState* my_extra = (ArrayMetaState*)fcinfo->flinfo->fn_extra;
    bool isnull;
    Datum value;

    EncInt* pMin = (EncInt*)palloc0(sizeof(EncInt));
    EncInt pTemp;
    array_iterator = array_create_iterator(v, 0, my_extra);
    array_iterate(array_iterator, &value, &isnull);

    memcpy(pMin, DatumGetCString(value), sizeof(EncInt));

    while (array_iterate(array_iterator, &value, &isnull)) {
        memcpy(&pTemp, DatumGetCString(value), sizeof(EncInt));

        enc_int_cmp(pMin, &pTemp, &ans);

        if (ans == 1)
            memcpy(pMin, &pTemp, sizeof(EncInt));
    }

    // pfree(pTemp);

    PG_RETURN_CSTRING(pMin);
}

Datum enc_int4_max_bulk(PG_FUNCTION_ARGS)
{
    ArrayType* v = PG_GETARG_ARRAYTYPE_P(0);
    int ans = 0;
    ArrayIterator array_iterator;
    ArrayMetaState* my_extra = (ArrayMetaState*)fcinfo->flinfo->fn_extra;
    bool isnull;
    Datum value;

    EncInt* pMax = (EncInt*)palloc0(sizeof(EncInt));
    EncInt pTemp;

    array_iterator = array_create_iterator(v, 0, my_extra);
    array_iterate(array_iterator, &value, &isnull);

    memcpy(pMax, DatumGetCString(value), sizeof(EncInt));

    while (array_iterate(array_iterator, &value, &isnull)) {
        memcpy(&pTemp, DatumGetCString(value), sizeof(EncInt));

        enc_int_cmp(pMax, &pTemp, &ans);
        if (ans == -1)
            memcpy(pMax, &pTemp, sizeof(EncInt));
    }

    // pfree(pTemp);

    PG_RETURN_CSTRING(pMax);
}

// Datum int4_to_enc_int4(PG_FUNCTION_ARGS)
// {
//     
//     int in = PG_GETARG_INT32(0);
//     EncInt* out = (EncInt*)palloc0(sizeof(EncInt));
//     enc_int_encrypt(in, out);
//     // ereport(LOG, (errmsg("function encrypt, output: %s", ans)));
//     PG_RETURN_CSTRING(out);
// }

// Datum int8_to_enc_int4(PG_FUNCTION_ARGS)
// {
//     
//     int in = PG_GETARG_INT64(0);
//     EncInt* out = (EncInt*)palloc0(sizeof(EncInt));
//     enc_int_encrypt(in, out);
//     // ereport(LOG, (errmsg("function encrypt, output: %s", ans)));
//     PG_RETURN_CSTRING(out);
// }
