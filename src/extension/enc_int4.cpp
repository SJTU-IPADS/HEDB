/*
 * enc_int4.c : This library defines exported functions for the encrypted integer(4 bytes) datatype.
 * The library contains functions for the Postgresql extension 'encdb', including:
 *
 * encrypted integer type, format: BASE64(IV[12bytes]||AES-GCM(int)[4 bytes]||AUTHTAG[16 bytes])
 * (input size: 4 bytes; output size: 44 bytes; operators: +,-,*,/,%,>=,>,<=,<,=,!=; functions: SUM, AVG, MIN, MAX)
 */
#include <enc_float_ops.hpp>
#include <enc_int_ops.hpp>
#include <extension.hpp>
extern bool debugMode;

#ifdef __cplusplus
extern "C" {
#endif
PG_FUNCTION_INFO_V1(pg_enc_int4_in);
PG_FUNCTION_INFO_V1(pg_enc_int4_out);
PG_FUNCTION_INFO_V1(pg_enc_int4_add);
PG_FUNCTION_INFO_V1(pg_enc_int4_sub);
PG_FUNCTION_INFO_V1(pg_enc_int4_mult);
PG_FUNCTION_INFO_V1(pg_enc_int4_div);
PG_FUNCTION_INFO_V1(pg_enc_int4_pow);
PG_FUNCTION_INFO_V1(pg_enc_int4_mod);
PG_FUNCTION_INFO_V1(pg_enc_int4_cmp);
PG_FUNCTION_INFO_V1(pg_enc_int4_eq);
PG_FUNCTION_INFO_V1(pg_enc_int4_ne);
PG_FUNCTION_INFO_V1(pg_enc_int4_lt);
PG_FUNCTION_INFO_V1(pg_enc_int4_le);
PG_FUNCTION_INFO_V1(pg_enc_int4_gt);
PG_FUNCTION_INFO_V1(pg_enc_int4_ge);
PG_FUNCTION_INFO_V1(pg_enc_int4_encrypt);
PG_FUNCTION_INFO_V1(pg_enc_int4_decrypt);
PG_FUNCTION_INFO_V1(pg_enc_int4_max);
PG_FUNCTION_INFO_V1(pg_enc_int4_min);
PG_FUNCTION_INFO_V1(pg_enc_int4_sum_bulk);
// PG_FUNCTION_INFO_V1(pg_enc_int4_avgfinal);
PG_FUNCTION_INFO_V1(pg_enc_int4_avg_bulk);
PG_FUNCTION_INFO_V1(pg_enc_int4_min_bulk);
PG_FUNCTION_INFO_V1(pg_enc_int4_max_bulk);
PG_FUNCTION_INFO_V1(pg_int4_to_enc_int4);
PG_FUNCTION_INFO_V1(pg_int8_to_enc_int4);
#ifdef __cplusplus
}
#endif

/*
 * The function converts string to enc_int4. It is called by dbms every time it parses a query and finds an enc_int4 element.
 * If flag debugMode is true it tries to convert input to int4 and encrypt it
 * @input: string as a postgres arg
 * @return: enc_int4 element as a string
 */
Datum pg_enc_int4_in(PG_FUNCTION_ARGS)
{
    char* pIn = PG_GETARG_CSTRING(0);
    EncInt* result = (EncInt*)palloc0(sizeof(EncInt));
    // int status = 0;

    int in = pg_atoi(pIn, INT32_LENGTH, '\0');
    enc_int_encrypt(in, result);
    // ereport(INFO, (errmsg("encrypt return")));

    PG_RETURN_POINTER(result);
}

/*
 * The function converts enc_int4 element to a string. If flag debugMode is true it decrypts the string and return unencrypted result.
 * @input: enc_int4 element
 * @return: string
 */
Datum pg_enc_int4_out(PG_FUNCTION_ARGS)
{
    EncInt* in = PG_GETARG_ENCINT(0);
    int out;
    char* str = (char*)palloc0(sizeof(EncInt)); // this length is not really meaningful

    if (debugMode == true) {
        enc_int_decrypt(in, &out);
        sprintf(str, "%d", out);
        // ereport(INFO, (errmsg("auto decryption: DEC('%p') = %d", in, out)));
    } else {
        memcpy(str, in, sizeof(EncInt));
    }

    PG_RETURN_CSTRING(str);
}

/*
 * The function calculates the sum of two enc_int4 values. It is called by binary operator '+' defined in sql extension.
 * It requires a running SGX enclave and uses the function 'enc_int_add' from the 'interface' library.
 * @input: two enc_int4 values
 * @return: encrypted sum of input values
 * output format: BASE64(iv[12 bytes]||AES-GCM(s1+s2)[4 bytes]||AUTHTAG[16bytes])
 */
Datum pg_enc_int4_add(PG_FUNCTION_ARGS)
{
    // int resp = 0;
    EncInt* left = PG_GETARG_ENCINT(0);
    EncInt* right = PG_GETARG_ENCINT(1);
    EncInt* result = (EncInt*)palloc0(sizeof(EncInt));

    enc_int_add(left, right, result);

    PG_RETURN_CSTRING(result);
}

/*
 * The function calculates the subtraction of two enc_int4 values. It is called by binary operator '-' defined in sql extension.
 * It requires a running SGX enclave and uses the function 'enc_int_sub' from the 'interface' library.
 * @input: two enc_int4 values
 * @return: an encrypted result of input values . output format: BASE64(iv[12 bytes]||AES-GCM(s1+s2)[4 bytes]||AUTHTAG[16bytes])
 */
Datum pg_enc_int4_sub(PG_FUNCTION_ARGS)
{
    // int resp = 0;
    EncInt* left = PG_GETARG_ENCINT(0);
    EncInt* right = PG_GETARG_ENCINT(1);
    EncInt* result = (EncInt*)palloc0(sizeof(EncInt));

    enc_int_sub(left, right, result);

    PG_RETURN_CSTRING(result);
}

/*
 * The function calculates the product of two enc_int4 values. It is called by binary operator '*' defined in sql extension.
 * It requires a running SGX enclave and uses the function 'enc_int_mult' from the 'interface' library.
 * @input: two enc_int4 values
 * @return: an encrypted result of input values . output format: BASE64(iv[12 bytes]||AES-GCM(s1+s2)[4 bytes]||AUTHTAG[16bytes])
 */
Datum pg_enc_int4_mult(PG_FUNCTION_ARGS)
{
    // int resp = 0;
    EncInt* left = PG_GETARG_ENCINT(0);
    EncInt* right = PG_GETARG_ENCINT(1);
    EncInt* result = (EncInt*)palloc0(sizeof(EncInt));

    enc_int_mult(left, right, result);

    PG_RETURN_CSTRING(result);
}

/*
 * The function calculates the division of two enc_int4 values. It is called by binary operator '/' defined in sql extension.
 * It requires a running SGX enclave and uses the function 'enc_int_div' from the 'interface' library.
 * @input: two enc_int4 values
 * @return: an encrypted result of input values . output format: BASE64(iv[12 bytes]||AES-GCM(s1+s2)[4 bytes]||AUTHTAG[16bytes])
 */
Datum pg_enc_int4_div(PG_FUNCTION_ARGS)
{
    // int resp = 0;
    EncInt* left = PG_GETARG_ENCINT(0);
    EncInt* right = PG_GETARG_ENCINT(1);
    EncInt* result = (EncInt*)palloc0(sizeof(EncInt));

    enc_int_div(left, right, result);

    PG_RETURN_CSTRING(result);
}

/*
 * The function calculates the first input enc_int4 value to the power of the second input enc_int4 value.
 * It is called by binary operator '^' defined in sql extension.
 * It requires a running SGX enclave and uses the function 'enc_int_pow' from the 'interface' library.
 * @input: two enc_int4 values
 * @return: an encrypted result of input values . output format: BASE64(iv[12 bytes]||AES-GCM(s1+s2)[4 bytes]||AUTHTAG[16bytes])
 */
Datum pg_enc_int4_pow(PG_FUNCTION_ARGS)
{
    // int resp = 0;
    EncInt* left = PG_GETARG_ENCINT(0);
    EncInt* right = PG_GETARG_ENCINT(1);
    EncInt* result = (EncInt*)palloc0(sizeof(EncInt));

    enc_int_pow(left, right, result);

    PG_RETURN_CSTRING(result);
}

/*
 * The function calculates the first input enc_int4 value by module the second input enc_int4 value.
 * It is called by binary operator '%' defined in sql extension.
 * It requires a running SGX enclave and uses the function 'enc_int_mod' from the 'interface' library.
 * @input: two enc_int4 values
 * @return: an encrypted result of input values . output format: BASE64(iv[12 bytes]||AES-GCM(s1+s2)[4 bytes]||AUTHTAG[16bytes])
 */
Datum pg_enc_int4_mod(PG_FUNCTION_ARGS)
{
    // int resp = 0;
    EncInt* left = PG_GETARG_ENCINT(0);
    EncInt* right = PG_GETARG_ENCINT(1);
    EncInt* result = (EncInt*)palloc0(sizeof(EncInt));

    enc_int_mod(left, right, result);

    PG_RETURN_CSTRING(result);
}

/*
 * The function compares two enc_int4 values. It is called mostly during index building.
 * It requires a running SGX enclave and uses the function 'enc_int_cmp' from the 'interface' library.
 * @input: two enc_int4 values
 * @return: -1, 0 ,1
 */
Datum pg_enc_int4_cmp(PG_FUNCTION_ARGS)
{
    // int resp = 0;
    EncInt* left = PG_GETARG_ENCINT(0);
    EncInt* right = PG_GETARG_ENCINT(1);
    int res;

    // int l = 1, r= 2;
    // bool cmp;
    // for(int i = 0; i <= 1000000; i ++){
    enc_int_cmp(left, right, &res);
    // cmp = l > r ? 1 :
    // l < r ? -1: 0;
    // }

    PG_RETURN_INT32(res);
}

/*
 * The function checks if the first input enc_int4 is equal to the second one.
 * It is called by binary operator '=' defined in sql extension.
 * It requires a running SGX enclave and uses the function 'enc_int_cmp' from the 'interface' library.
 * @input: two enc_int4 values
 * @return: true, if the first decrypted integer is equal to the second one.
 *       false, otherwise
 */
Datum pg_enc_int4_eq(PG_FUNCTION_ARGS)
{
    // int resp = 0;
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

/*
 * The function checks if the first input enc_int4 is not equal to the second one.
 * It is called by binary operator '!=' defined in sql extension.
 * It requires a running SGX enclave and uses the function 'enc_int_cmp' from the 'interface' library.
 * @input: two enc_int4 values
 * @return: true, if the first decrypted integer is not equal to the second one.
 *       false, otherwise
 */
Datum pg_enc_int4_ne(PG_FUNCTION_ARGS)
{
    // int resp = 0;
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

/*
 * The function checks if the first input enc_int4 is less than the second one.
 * It is called by binary operator '<' defined in sql extension.
 * It requires a running SGX enclave and uses the function 'enc_int_cmp' from the 'interface' library.
 * @input: two enc_int4 values
 * @return: true, if the first decrypted integer is less the the second one.
 *       false, otherwise
 */
Datum pg_enc_int4_lt(PG_FUNCTION_ARGS)
{
    // int resp = 0;
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

/*
 * The function checks if the first input enc_int4 is less or equal than the second one.
 * It is called by binary operator '<=' defined in sql extension.
 * It requires a running SGX enclave and uses the function 'enc_int_cmp' from the 'interface' library.
 * @input: two enc_int4 values
 * @return: true, if the first decrypted integer is less or equal than the second one.
 *       false, otherwise
 */
Datum pg_enc_int4_le(PG_FUNCTION_ARGS)
{
    // int resp = 0;
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

/*
 * The function checks if the first input enc_int4 is greater than the second one.
 * It is called by binary operator '>' defined in sql extension.
 * It requires a running SGX enclave and uses the function 'enc_int_cmp' from the 'interface' library.
 * @input: two enc_int4 values
 * @return: true, if the first decrypted integer is greater than the second one.
 *          false, otherwise
 */
Datum pg_enc_int4_gt(PG_FUNCTION_ARGS)
{
    // int resp = 0;
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

/*
 * The function checks if the first input enc_int4 is greater or equal than the second one.
 * It is called by binary operator '>=' defined in sql extension.
 * It requires a running SGX enclave and uses the function 'enc_int_cmp' from the 'interface' library.
 * @input: two enc_int4 values
 * @return: true, if the first decrypted integer is greater or equal than the second one.
 *          false, otherwise
 */
Datum pg_enc_int4_ge(PG_FUNCTION_ARGS)
{
    // int resp = 0;
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

// DEBUG FUNCTION, WILL BE DELETED IN THE PRODUCT
Datum pg_enc_int4_encrypt(PG_FUNCTION_ARGS)
{
    EncInt* out = (EncInt*)palloc0(sizeof(EncInt));
    int in = PG_GETARG_INT32(0);
    enc_int_encrypt(in, out);
    // ereport(LOG, (errmsg("function encrypt, output: %s", ans)));
    PG_RETURN_CSTRING(out);
}

// WILL BE DELETED IN THE PRODUCT
Datum pg_enc_int4_decrypt(PG_FUNCTION_ARGS)
{
    int ans = 0;
    EncInt* in = PG_GETARG_ENCINT(0);
    enc_int_decrypt(in, &ans);
    // ereport(LOG, (errmsg("function decrypt, output: %d", ans)));
    PG_RETURN_INT32(ans);
}

/*
 * The function
 * @input: two enc_int4 values
 * @return: the larger enc_int4 value.
 */
Datum pg_enc_int4_max(PG_FUNCTION_ARGS)
{
    // int resp = 0;
    EncInt* left = PG_GETARG_ENCINT(0);
    EncInt* right = PG_GETARG_ENCINT(1);
    int res;
    // bool cmp = false;

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
Datum pg_enc_int4_min(PG_FUNCTION_ARGS)
{
    // int resp = 0;
    EncInt* left = PG_GETARG_ENCINT(0);
    EncInt* right = PG_GETARG_ENCINT(1);
    int res;
    // bool cmp = false;

    enc_int_cmp(left, right, &res);

    if (res == 1) {
        PG_RETURN_POINTER(right);
    } else {
        PG_RETURN_POINTER(left);
    }
}

Datum pg_enc_int4_sum_bulk(PG_FUNCTION_ARGS)
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

Datum pg_enc_int4_avg_bulk(PG_FUNCTION_ARGS)
{
    ArrayType* v = PG_GETARG_ARRAYTYPE_P(0);
    bool isnull;
    Datum value;
    int ndims1 = ARR_NDIM(v); // array dimension
    int* dims1 = ARR_DIMS(v);
    int nitems = ArrayGetNItems(ndims1, dims1); // number of items in array

    EncInt sum;
    EncInt* res = (EncInt*)palloc0(sizeof(EncInt));
    EncInt array[BULK_SIZE];
    EncInt num;
    int counter; // sum will be at array[0]

    ArrayMetaState* my_extra = (ArrayMetaState*)fcinfo->flinfo->fn_extra;
    ArrayIterator array_iterator = array_create_iterator(v, 0, my_extra);

    array_iterate(array_iterator, &value, &isnull);
    sum = *DatumGetEncInt(value);
    array[0] = sum;
    counter = 1;
    while (array_iterate(array_iterator, &value, &isnull)) {
        array[counter] = *DatumGetEncInt(value);
        counter++;
        if (counter == BULK_SIZE) {
            enc_int_sum_bulk(BULK_SIZE, array, &sum);
            array[0] = sum;
            counter = 1;
        }
    }

    if (counter > 1) {
        enc_int_sum_bulk(counter, array, &sum);
    }
    enc_int_encrypt(nitems, &num);
    enc_int_div(&sum, &num, res);

    PG_RETURN_CSTRING(res);
}

Datum pg_enc_int4_min_bulk(PG_FUNCTION_ARGS)
{
    ArrayType* v = PG_GETARG_ARRAYTYPE_P(0);
    // int resp = 0;
    int ans = 0;
    ArrayIterator array_iterator;
    ArrayMetaState* my_extra = (ArrayMetaState*)fcinfo->flinfo->fn_extra;
    bool isnull;
    Datum value;

    // int ndims1 = ARR_NDIM(v); //array dimension
    // int* dims1 = ARR_DIMS(v);
    // int nitems = ArrayGetNItems(ndims1, dims1); //number of items in array

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

Datum pg_enc_int4_max_bulk(PG_FUNCTION_ARGS)
{
    ArrayType* v = PG_GETARG_ARRAYTYPE_P(0);
    // int resp = 0;
    int ans = 0;
    ArrayIterator array_iterator;
    ArrayMetaState* my_extra = (ArrayMetaState*)fcinfo->flinfo->fn_extra;
    bool isnull;
    Datum value;

    // int ndims1 = ARR_NDIM(v); //array dimension
    // int* dims1 = ARR_DIMS(v);
    // int nitems = ArrayGetNItems(ndims1, dims1); //number of items in array

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

/*
 * The function converts an integer to enc_int4 value. This function is calles by sql function CAST.
 * It requires a running SGX enclave and uses the function 'enc_int_encrypt' from the 'interface' library.
 * @input: int4
 * @return: an encrypted result. output format: BASE64(iv[12 bytes]||AES-GCM(s1+s2)[4 bytes]||AUTHTAG[16bytes])
 */
Datum pg_int4_to_enc_int4(PG_FUNCTION_ARGS)
{
    int in = PG_GETARG_INT32(0);
    EncInt* out = (EncInt*)palloc0(sizeof(EncInt));
    enc_int_encrypt(in, out);
    // ereport(LOG, (errmsg("function encrypt, output: %s", ans)));
    PG_RETURN_CSTRING(out);
}

/*
 * The function converts an integer(8 bytes, known as bigint) to enc_int4 value. This function is calles by sql function CAST.
 * It requires a running SGX enclave and uses the function 'enc_int_encrypt' from the 'interface' library.
 * @input: int8
 * @return: an encrypted result. output format: BASE64(iv[12 bytes]||AES-GCM(s1+s2)[4 bytes]||AUTHTAG[16bytes])
 */
Datum pg_int8_to_enc_int4(PG_FUNCTION_ARGS)
{

    int in = PG_GETARG_INT64(0);
    EncInt* out = (EncInt*)palloc0(sizeof(EncInt));
    enc_int_encrypt(in, out);
    // ereport(LOG, (errmsg("function encrypt, output: %s", ans)));
    PG_RETURN_CSTRING(out);
}
