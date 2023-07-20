#include "base64.h"
#include <enc_float_ops.hpp>
#include <extension.hpp>
#include <string.h>

extern bool clientMode;

#ifdef __cplusplus
extern "C" {
#endif
PG_FUNCTION_INFO_V1(enc_float4_in);
PG_FUNCTION_INFO_V1(enc_float4_out);
PG_FUNCTION_INFO_V1(enc_float4_encrypt);
PG_FUNCTION_INFO_V1(enc_float4_decrypt);
PG_FUNCTION_INFO_V1(enc_float4_sum_bulk);
PG_FUNCTION_INFO_V1(enc_float4_avg_bulk);
PG_FUNCTION_INFO_V1(enc_float4_eval_expr);
PG_FUNCTION_INFO_V1(enc_float4_avg_simple);
PG_FUNCTION_INFO_V1(enc_float4_min);
PG_FUNCTION_INFO_V1(enc_float4_max);
PG_FUNCTION_INFO_V1(enc_float4_add);
PG_FUNCTION_INFO_V1(enc_float4_subs);
PG_FUNCTION_INFO_V1(enc_float4_mult);
PG_FUNCTION_INFO_V1(enc_float4_div);
PG_FUNCTION_INFO_V1(enc_float4_exp);
PG_FUNCTION_INFO_V1(enc_float4_eq);
PG_FUNCTION_INFO_V1(enc_float4_ne);
PG_FUNCTION_INFO_V1(enc_float4_lt);
PG_FUNCTION_INFO_V1(enc_float4_le);
PG_FUNCTION_INFO_V1(enc_float4_gt);
PG_FUNCTION_INFO_V1(enc_float4_ge);
PG_FUNCTION_INFO_V1(enc_float4_cmp);
PG_FUNCTION_INFO_V1(enc_float4_mod);
PG_FUNCTION_INFO_V1(float4_to_enc_float4);
PG_FUNCTION_INFO_V1(numeric_to_enc_float4);
PG_FUNCTION_INFO_V1(double_to_enc_float4);
PG_FUNCTION_INFO_V1(int8_to_enc_float4);
PG_FUNCTION_INFO_V1(int4_to_enc_float4);
#ifdef __cplusplus
}
#endif

float4 pg_float4_in(char* num);

#ifdef _MSC_VER
#pragma warning(disable : 4756)
#endif
static inline float4
get_float4_infinity(void)
{
#ifdef INFINITY
    /* C99 standard way */
    return (float4)INFINITY;
#else
#ifdef _MSC_VER
#pragma warning(default : 4756)
#endif

    /*
     * On some platforms, HUGE_VAL is an infinity, elsewhere it's just the
     * largest normal float8.  We assume forcing an overflow will get us a
     * true infinity.
     */
    return (float4)(HUGE_VAL * HUGE_VAL);
#endif
}

static inline float4
get_float4_nan(void)
{
#ifdef NAN
    /* C99 standard way */
    return (float4)NAN;
#else
    /* Assume we can get a NAN via zero divide */
    return (float4)(0.0 / 0.0);
#endif
}

/*
 * The function converts string to enc_float4. It is called by dbms every time it parses a query and finds an enc_float4 element.
 * It uses function pg_float4_in to convert it to float4 and returns an error if it can't
 * @input: string as a postgres arg
 * @return: enc_float4 element as a string
 */
Datum enc_float4_in(PG_FUNCTION_ARGS)
{
    char* s = PG_GETARG_CSTRING(0);
    float val = pg_float4_in(s);
    EncFloat* f = (EncFloat*)palloc0(sizeof(EncFloat));
    enc_float_encrypt(val, f);

    PG_RETURN_POINTER(f);
}

/*
 * The function converts enc_float4 element to a string.
 * @input: enc_float4 element
 * @return: string
 */
Datum enc_float4_out(PG_FUNCTION_ARGS)
{
    EncFloat* in = PG_GETARG_ENCFlOAT(0);

    if (clientMode == true) {
        char* str = (char*)palloc0(sizeof(EncFloat));
        float out;
        enc_float_decrypt(in, &out);
        sprintf(str, "%f", out);
        PG_RETURN_POINTER(str);
    } else {
#define ENC_FLOAT_B64_LENGTH 45 // ((4 * n / 3) + 3) & ~3
        char base64_float4[ENC_FLOAT_B64_LENGTH + 1] = { 0 };

        toBase64((const unsigned char*)in, sizeof(EncFloat), base64_float4);
        // ereport(INFO, (errmsg("base64('%p') = %s", in, base64_float4)));
        PG_RETURN_CSTRING(base64_float4);
    }
}

/*
 * The function converts a float to enc_float4 value. This function is called by sql function CAST.
 * @input: float4
 * @return: an encrypted result.
 */
Datum float4_to_enc_float4(PG_FUNCTION_ARGS)
{
    float src = PG_GETARG_FLOAT4(0);
    EncFloat* f = (EncFloat*)palloc0(sizeof(EncFloat));
    enc_float_encrypt(src, f);
    PG_RETURN_POINTER(f);
}

/*
 * The function converts a numeric datatype(postgres variable datatype can be any of int2, int4, int8, float4, float8) to enc_float4 value.
 * This function is called by sql function CAST. It uses function pg_float4_in to convert it to float4 and return an error if it can't
 * @input: float4
 * @return: an enc_float4 result.
 */
Datum numeric_to_enc_float4(PG_FUNCTION_ARGS)
{
    Numeric num = PG_GETARG_NUMERIC(0);
    EncFloat* f = (EncFloat*)palloc0(sizeof(EncFloat));
    float4 src;
    char* tmp = DatumGetCString(DirectFunctionCall1(numeric_out, NumericGetDatum(num)));

    src = pg_float4_in(tmp);
    enc_float_encrypt(src, f);
    // pfree(tmp);
    PG_RETURN_POINTER(f);
}

/*
 * The function converts a double precision datatype to enc_float4 value.
 * This function is called by sql function CAST. It uses function pg_float4_in to convert it to float4 and return an error if it can't
 * @input: float8
 * @return: an enc_float4 result.
 */
Datum double_to_enc_float4(PG_FUNCTION_ARGS)
{
    float8 num = PG_GETARG_FLOAT8(0);
    EncFloat* f = (EncFloat*)palloc0(sizeof(EncFloat));
    float src;
    char* tmp = DatumGetCString(DirectFunctionCall1(float8out, Float8GetDatum(num)));

    src = pg_float4_in(tmp);
    enc_float_encrypt(src, f);
    // pfree(tmp);
    PG_RETURN_POINTER(f);
}

/*
 * The function converts a bigint (int8) datatype to enc_float4 value.
 * This function is called by sql function CAST. It uses function pg_float4_in to convert it to float4 and return an error if it can't
 * @input: int8
 * @return: an enc_float4 result.
 */
Datum int8_to_enc_float4(PG_FUNCTION_ARGS)
{
    int8 num = PG_GETARG_INT64(0);
    EncFloat* f = (EncFloat*)palloc0(sizeof(EncFloat));
    float4 src;
    char* tmp = DatumGetCString(DirectFunctionCall1(int8out, Int8GetDatum(num)));

    src = pg_float4_in(tmp);
    enc_float_encrypt(src, f);
    // pfree(tmp);
    PG_RETURN_POINTER(f);
}

/*
 * The function converts a int (int4) datatype to enc_float4 value.
 * This function is called by sql function CAST. It uses function pg_float4_in to convert it to float4 and return an error if it can't
 * @input: int4
 * @return: an enc_float4 result.
 */
Datum int4_to_enc_float4(PG_FUNCTION_ARGS)
{
    int num = PG_GETARG_INT32(0);
    EncFloat* f = (EncFloat*)palloc0(sizeof(EncFloat));
    float4 src;
    char* tmp = DatumGetCString(DirectFunctionCall1(int4out, Int32GetDatum(num)));

    src = pg_float4_in(tmp);
    enc_float_encrypt(src, f);
    // pfree(tmp);
    PG_RETURN_POINTER(f);
}

// TODO
//  DEBUG FUNCTION
//  WILL BE DELETED IN THE PRODUCT
Datum enc_float4_encrypt(PG_FUNCTION_ARGS)
{
    float src = PG_GETARG_FLOAT4(0);
    EncFloat* f = (EncFloat*)palloc0(sizeof(EncFloat));
    enc_float_encrypt(src, f);
    PG_RETURN_CSTRING(f);
}

// TODO
//  DEBUG FUNCTION
//  WILL BE DELETED IN THE PRODUCT
Datum enc_float4_decrypt(PG_FUNCTION_ARGS)
{
    EncFloat* s = PG_GETARG_ENCFlOAT(0);
    float ans;
    enc_float_decrypt(s, &ans);
    PG_RETURN_FLOAT4(ans);
}

Datum enc_float4_sum_bulk(PG_FUNCTION_ARGS)
{
    ArrayType* v = PG_GETARG_ARRAYTYPE_P(0);
    bool isnull;
    Datum value;
    EncFloat* sum = (EncFloat*)palloc0(sizeof(EncFloat));
    EncFloat array[BULK_SIZE];
    int counter = 1; // sum will be at array[0]

    ArrayMetaState* my_extra = (ArrayMetaState*)fcinfo->flinfo->fn_extra;
    ArrayIterator array_iterator = array_create_iterator(v, 0, my_extra);

    array_iterate(array_iterator, &value, &isnull);
    *sum = *DatumGetEncFloat(value);
    array[0] = *sum;
    while (array_iterate(array_iterator, &value, &isnull)) {
        array[counter] = *DatumGetEncFloat(value);
        counter++;
        if (counter == BULK_SIZE) {
            enc_float_sum_bulk(BULK_SIZE, array, sum);
            array[0] = *sum;
            counter = 1;
        }
    }
    if (counter > 1) {
        enc_float_sum_bulk(counter, array, sum);
    }

    PG_RETURN_POINTER(sum);
}

Datum enc_float4_avg_bulk(PG_FUNCTION_ARGS)
{
    // print_info("avg bulk");
    ArrayType* v = PG_GETARG_ARRAYTYPE_P(0);
    bool isnull;
    Datum value;
    int ndims1 = ARR_NDIM(v); // array dimension
    int* dims1 = ARR_DIMS(v);
    int nitems = ArrayGetNItems(ndims1, dims1); // number of items in array

    EncFloat sum;
    EncFloat* res = (EncFloat*)palloc0(sizeof(EncFloat));
    EncFloat num;
    EncFloat array[BULK_SIZE];
    int counter; // sum will be at array[0]

    ArrayMetaState* my_extra = (ArrayMetaState*)fcinfo->flinfo->fn_extra;
    ArrayIterator array_iterator = array_create_iterator(v, 0, my_extra);

    array_iterate(array_iterator, &value, &isnull);
    sum = *DatumGetEncFloat(value);
    array[0] = sum;
    counter = 1;
    while (array_iterate(array_iterator, &value, &isnull)) {
        array[counter] = *DatumGetEncFloat(value);
        counter++;
        if (counter == BULK_SIZE) {
            enc_float_sum_bulk(BULK_SIZE, array, &sum);
            array[0] = sum;
            counter = 1;
        }
    }
    if (counter > 1) {
        enc_float_sum_bulk(counter, array, &sum);
    }
    enc_float_encrypt(nitems * 1.0, &num);
    enc_float_div(&sum, &num, res);
    PG_RETURN_POINTER(res);
}

char* remove_space(char* expr)
{
    int i, j;
    char* expr_no_space = expr;
    for (i = 0, j = 0; i < strlen(expr); i++, j++) {
        if (expr[i] != ' ')
            expr_no_space[j] = expr[i];
        else
            j--;
    }
    expr_no_space[j] = '\0';
    return expr_no_space;
}

int get_precedance(const int op)
{
    if (op == '+' || op == '-')
        return 0;
    if (op == '*' || op == '/' || op == '%')
        return 1;
    if (op == '^')
        return 2;
    return -1;
}

/**
 * @brief implementation of shunting yard algorithm
 *
 * @param expr
 * @param out_expr
 */
void convert_expr(char* expr, char* out_expr)
{
    int num, op_top_pos = -1;
    size_t i, out_len = 0;
    char c, op_top;
    char op_stack[EXPR_STACK_MAX_SIZE];
    int8_t out_queue[EXPR_STACK_MAX_SIZE];
    memset(out_queue, 0, (size_t)EXPR_STACK_MAX_SIZE);
    memset(op_stack, 0, (size_t)EXPR_STACK_MAX_SIZE);
    bool expect_op = false;
    for (i = 0; i < strlen(expr); ++i) {
        c = expr[i];
        if (c >= '0' && c <= '9') {
            num = c - '0';
            if (i + 1 < strlen(expr)) {
                while (expr[i + 1] >= '0' && expr[i + 1] <= '9') {
                    num = 10 * num + expr[i + 1] - '0';
                    i++;
                    if (i + 1 >= strlen(expr)) {
                        break;
                    }
                }
            }
            out_queue[out_len] = (int8_t)num;
            out_len++;
            expect_op = true;
        } else if (c == '+' || c == '-' || c == '*' || c == '/' || c == '%' || c == '^') {
            if (op_top_pos >= 0) {
                op_top = op_stack[op_top_pos];
                while (op_top != '(' && c != '^' && get_precedance(op_top) >= get_precedance(c) && op_top_pos >= 0) {
                    out_queue[out_len] = (int8_t)-op_top;
                    op_top_pos--;
                    op_top = op_stack[op_top_pos];
                    out_len++;
                }
            }
            if (!expect_op && c == '-') {
                c = '#';
            }
            op_top_pos++;
            op_stack[op_top_pos] = c;
            expect_op = false;
        } else if (c == '(') {
            op_top_pos++;
            op_stack[op_top_pos] = c;
            expect_op = false;
        } else if (c == ')') {
            do {
                if (op_top_pos < 0) {
                    printf("Mismatched parenthesis!\n");
                    exit(0);
                }
                op_top = op_stack[op_top_pos];
                if (op_top == '(') {
                    op_top_pos--;
                    break;
                }
                out_queue[out_len] = (int8_t)-op_top;
                op_top_pos--;
                out_len++;
            } while (1);
            expect_op = false;
        }
    }
    while (op_top_pos >= 0) {
        op_top = op_stack[op_top_pos];
        if (op_top == '(') {
            printf("Extra parenthesis!\n");
            exit(0);
        }
        out_queue[out_len] = (int8_t)-op_top;
        op_top_pos--;
        out_len++;
    }
    memcpy(out_expr, out_queue, out_len + 1);
}

Datum enc_float4_eval_expr(PG_FUNCTION_ARGS)
{
    // print_info("eval expr");
    Datum* args;
    bool* nulls;
    Oid* types;
    int i;
    EncFloat* arr[EXPR_MAX_SIZE];
    EncFloat* res = (EncFloat*)palloc0(sizeof(EncFloat));
    char *s, s_postfix[EXPR_STACK_MAX_SIZE];
    Str* str = (Str*)palloc0(sizeof(Str));
    memset(s_postfix, 0, (size_t)EXPR_STACK_MAX_SIZE);

    int nargs = extract_variadic_args(fcinfo, 0, true, &args, &types, &nulls);

    if (nargs < 0)
        PG_RETURN_NULL();

    // ereport(INFO, (errmsg("nargs: %d expr: %s", nargs, s)));
    s = PG_GETARG_CSTRING(0);
    s = remove_space(s);
    convert_expr(s, s_postfix);

    for (i = 1; i < nargs; i++) {
        arr[i - 1] = DatumGetEncFloat(args[i]);
        // enc_float_decrypt(arr[i-1], &tmp);
        // ereport(INFO, (errmsg("%f", tmp)));
    }

    str->len = strlen(s_postfix);
    memcpy(str->data, s_postfix, str->len);

    enc_float_eval_expr(nargs - 1, *str, arr, res);
    pfree(str);
    PG_RETURN_POINTER(res);
}

Datum enc_float4_avg_simple(PG_FUNCTION_ARGS)
{
    ArrayType* v = PG_GETARG_ARRAYTYPE_P(0);
    bool isnull;
    Datum value;
    int ndims1 = ARR_NDIM(v); // array dimension
    int* dims1 = ARR_DIMS(v);
    int nitems = ArrayGetNItems(ndims1, dims1); // number of items in array

    EncFloat* sum = (EncFloat*)palloc0(sizeof(EncFloat));
    EncFloat* res = (EncFloat*)palloc0(sizeof(EncFloat));
    EncFloat num, tmp;

    ArrayMetaState* my_extra = (ArrayMetaState*)fcinfo->flinfo->fn_extra;
    ArrayIterator array_iterator = array_create_iterator(v, 0, my_extra);

    array_iterate(array_iterator, &value, &isnull);
    *sum = *DatumGetEncFloat(value);
    while (array_iterate(array_iterator, &value, &isnull)) {
        tmp = *DatumGetEncFloat(value);
        enc_float_add(sum, &tmp, sum);
    }

    enc_float_encrypt(nitems * 1.0, &num);
    enc_float_div(sum, &num, res);
    pfree(sum);
    PG_RETURN_POINTER(res);
}
/*
 * return the less between the two.
 */
Datum enc_float4_max(PG_FUNCTION_ARGS)
{
    EncFloat* f1 = PG_GETARG_ENCFlOAT(0);
    EncFloat* f2 = PG_GETARG_ENCFlOAT(1);
    int cmp;

    enc_float_cmp(f1, f2, &cmp);
    if (cmp == 1)
        PG_RETURN_POINTER(f1);
    else
        PG_RETURN_POINTER(f2);
}
/*
 * return the less between the two.
 */
Datum enc_float4_min(PG_FUNCTION_ARGS)
{
    EncFloat* f1 = PG_GETARG_ENCFlOAT(0);
    EncFloat* f2 = PG_GETARG_ENCFlOAT(1);
    int cmp;

    enc_float_cmp(f1, f2, &cmp);
    if (cmp == 0)
        PG_RETURN_POINTER(f1);
    else
        PG_RETURN_POINTER(f2);
}

Datum enc_float4_add(PG_FUNCTION_ARGS)
{
    EncFloat* f1 = PG_GETARG_ENCFlOAT(0);
    EncFloat* f2 = PG_GETARG_ENCFlOAT(1);
    EncFloat* f = (EncFloat*)palloc0(sizeof(EncFloat));

    enc_float_add(f1, f2, f);

    PG_RETURN_POINTER(f);
}

/*
 * The function calculates the subtraction of two enc_float4 values. It is called by binary operator '-' defined in sql extension.
 * @input: two enc_float4 values
 * @return: result of input values .
 */
Datum enc_float4_subs(PG_FUNCTION_ARGS)
{
    EncFloat* f1 = PG_GETARG_ENCFlOAT(0);
    EncFloat* f2 = PG_GETARG_ENCFlOAT(1);
    EncFloat* f = (EncFloat*)palloc0(sizeof(EncFloat));

    enc_float_sub(f1, f2, f);

    PG_RETURN_POINTER(f);
}

/*
 * The function calculates the product of two enc_float4 values. It is called by binary operator '*' defined in sql extension.
 * @input: two enc_float4 values
 * @return: an enc_float4 result of input values .
 */
Datum enc_float4_mult(PG_FUNCTION_ARGS)
{
    EncFloat* f1 = PG_GETARG_ENCFlOAT(0);
    EncFloat* f2 = PG_GETARG_ENCFlOAT(1);
    EncFloat* f = (EncFloat*)palloc0(sizeof(EncFloat));

    enc_float_mult(f1, f2, f);

    PG_RETURN_POINTER(f);
}

/*
 * The function calculates the division of two enc_float4 values. It is called by binary operator '/' defined in sql extension.
 * @input: two enc_float4 values
 * @return: an enc_float4 result of input values .
 */

Datum enc_float4_div(PG_FUNCTION_ARGS)
{
    EncFloat* f1 = PG_GETARG_ENCFlOAT(0);
    EncFloat* f2 = PG_GETARG_ENCFlOAT(1);
    EncFloat* f = (EncFloat*)palloc0(sizeof(EncFloat));

    enc_float_div(f1, f2, f);

    PG_RETURN_POINTER(f);
}

/*
 * The function calculates the first input enc_float4 value to the power of the second input enc_float4 value.
 * It is called by binary operator '^' defined in sql extension.
 * @input: two enc_float4 values
 * @return: an enc_float4 result of input values .
 */
Datum enc_float4_exp(PG_FUNCTION_ARGS)
{
    EncFloat* f1 = PG_GETARG_ENCFlOAT(0);
    EncFloat* f2 = PG_GETARG_ENCFlOAT(1);
    EncFloat* f = (EncFloat*)palloc0(sizeof(EncFloat));

    enc_float_pow(f1, f2, f);

    PG_RETURN_POINTER(f);
}

/*
 * The function calculates the first input enc_float4 value by module the second input enc_float4 value.
 * It is called by binary operator '%' defined in sql extension.
 * @input: two enc_float4 values
 * @return: result of input values .
 */
Datum enc_float4_mod(PG_FUNCTION_ARGS)
{
    EncFloat* f1 = PG_GETARG_ENCFlOAT(0);
    EncFloat* f2 = PG_GETARG_ENCFlOAT(1);
    EncFloat* f = (EncFloat*)palloc0(sizeof(EncFloat));

    enc_float_mod(f1, f2, f);

    PG_RETURN_POINTER(f);
}
/*
 * The function checks if the first input enc_float4 is equal to the second one.
 * It is called by binary operator '=' defined in sql extension.
 * @input: two enc_float4 values
 * @return: true, if the first float is equal to the second one.
 *       false, otherwise
 */
Datum enc_float4_eq(PG_FUNCTION_ARGS)
{
    EncFloat* f1 = PG_GETARG_ENCFlOAT(0);
    EncFloat* f2 = PG_GETARG_ENCFlOAT(1);
    int cmp, ret;

    enc_float_cmp(f1, f2, &cmp);

    ret = cmp == 0;

    PG_RETURN_BOOL(ret);
}

/*
 * The function checks if the first input enc_float4 is not equal to the second one.
 * It is called by binary operator '!=' defined in sql extension.
 * @input: two enc_float4 values
 * @return: true, if the first float is not equal to the second one.
 *       false, otherwise
 */
Datum enc_float4_ne(PG_FUNCTION_ARGS)
{
    EncFloat* f1 = PG_GETARG_ENCFlOAT(0);
    EncFloat* f2 = PG_GETARG_ENCFlOAT(1);
    int cmp, ret;

    enc_float_cmp(f1, f2, &cmp);

    ret = cmp != 0;

    PG_RETURN_BOOL(ret);
}

/*
 * The function checks if the first input enc_float4 is less than the second one.
 * It is called by binary operator '<' defined in sql extension.
 * @input: two enc_float4 values
 * @return: true, if the first decrypted float is less the the second one.
 *       false, otherwise
 */
Datum enc_float4_lt(PG_FUNCTION_ARGS)
{
    EncFloat* f1 = PG_GETARG_ENCFlOAT(0);
    EncFloat* f2 = PG_GETARG_ENCFlOAT(1);
    int cmp, ret;

    enc_float_cmp(f1, f2, &cmp);

    ret = cmp == -1;

    PG_RETURN_BOOL(ret);
}

/*
 * The function checks if the first input enc_float4 is less or equal than the second one.
 * It is called by binary operator '<=' defined in sql extension.
 * @input: two enc_float4 values
 * @return: true, if the first enc_float4 is less or equal than the second one.
 *       false, otherwise
 */
Datum enc_float4_le(PG_FUNCTION_ARGS)
{
    EncFloat* f1 = PG_GETARG_ENCFlOAT(0);
    EncFloat* f2 = PG_GETARG_ENCFlOAT(1);
    int cmp, ret;

    enc_float_cmp(f1, f2, &cmp);

    ret = cmp <= 0;

    PG_RETURN_BOOL(ret);
}

/*
 * The function checks if the first input enc_float4 is greater than the second one.
 * It is called by binary operator '>' defined in sql extension.
 * @input: two enc_float4 values
 * @return: true, if the first float is greater than the second one.
 *          false, otherwise
 */
Datum enc_float4_gt(PG_FUNCTION_ARGS)
{
    EncFloat* f1 = PG_GETARG_ENCFlOAT(0);
    EncFloat* f2 = PG_GETARG_ENCFlOAT(1);
    int cmp, ret;

    enc_float_cmp(f1, f2, &cmp);

    ret = cmp == 1;

    PG_RETURN_BOOL(ret);
}

/*
 * The function checks if the first input enc_float4 is greater or equal than the second one.
 * It is called by binary operator '>=' defined in sql extension.
 * @input: two enc_float4 values
 * @return: true, if the first float is greater or equal than the second one.
 *          false, otherwise
 */
Datum enc_float4_ge(PG_FUNCTION_ARGS)
{
    EncFloat* f1 = PG_GETARG_ENCFlOAT(0);
    EncFloat* f2 = PG_GETARG_ENCFlOAT(1);
    int cmp, ret;

    enc_float_cmp(f1, f2, &cmp);

    ret = cmp >= 0;

    PG_RETURN_BOOL(ret);
}

/*
 * The function compares two enc_float4 values. It is called mostly during index building.
 * @input: two enc_float4 values
 * @return: -1, 0 ,1
 */
Datum enc_float4_cmp(PG_FUNCTION_ARGS)
{
    EncFloat* f1 = PG_GETARG_ENCFlOAT(0);
    EncFloat* f2 = PG_GETARG_ENCFlOAT(1);
    int cmp;

    enc_float_cmp(f1, f2, &cmp);

    PG_RETURN_INT32(cmp);
}

float4 pg_float4_in(char* num)
{
    char* orig_num;
    double val;
    char* endptr;

    /*
     * endptr points to the first character _after_ the sequence we recognized
     * as a valid floating point number. orig_num points to the original input
     * string.
     */
    orig_num = num;

    /* skip leading whitespace */
    while (*num != '\0' && isspace((unsigned char)*num))
        num++;

    /*
     * Check for an empty-string input to begin with, to avoid the vagaries of
     * strtod() on different platforms.
     */
    if (*num == '\0')
        ereport(ERROR,
            (errcode(ERRCODE_INVALID_TEXT_REPRESENTATION),
                errmsg("invalid input syntax for type %s: \"%s\"",
                    "real", orig_num)));

    errno = 0;
    val = strtod(num, &endptr);

    /* did we not see anything that looks like a double? */
    if (endptr == num || errno != 0) {
        int save_errno = errno;

        /*
         * C99 requires that strtod() accept NaN, [+-]Infinity, and [+-]Inf,
         * but not all platforms support all of these (and some accept them
         * but set ERANGE anyway...)  Therefore, we check for these inputs
         * ourselves if strtod() fails.
         *
         * Note: C99 also requires hexadecimal input as well as some extended
         * forms of NaN, but we consider these forms unportable and don't try
         * to support them.  You can use 'em if your strtod() takes 'em.
         */
        if (pg_strncasecmp(num, "NaN", 3) == 0) {
            val = get_float4_nan();
            endptr = num + 3;
        } else if (pg_strncasecmp(num, "Infinity", 8) == 0) {
            val = get_float4_infinity();
            endptr = num + 8;
        } else if (pg_strncasecmp(num, "+Infinity", 9) == 0) {
            val = get_float4_infinity();
            endptr = num + 9;
        } else if (pg_strncasecmp(num, "-Infinity", 9) == 0) {
            val = -get_float4_infinity();
            endptr = num + 9;
        } else if (pg_strncasecmp(num, "inf", 3) == 0) {
            val = get_float4_infinity();
            endptr = num + 3;
        } else if (pg_strncasecmp(num, "+inf", 4) == 0) {
            val = get_float4_infinity();
            endptr = num + 4;
        } else if (pg_strncasecmp(num, "-inf", 4) == 0) {
            val = -get_float4_infinity();
            endptr = num + 4;
        } else if (save_errno == ERANGE) {
            /*
             * Some platforms return ERANGE for denormalized numbers (those
             * that are not zero, but are too close to zero to have full
             * precision).  We'd prefer not to throw error for that, so try to
             * detect whether it's a "real" out-of-range condition by checking
             * to see if the result is zero or huge.
             */
            if (val == 0.0 || val >= HUGE_VAL || val <= -HUGE_VAL)
                ereport(ERROR,
                    (errcode(ERRCODE_NUMERIC_VALUE_OUT_OF_RANGE),
                        errmsg("\"%s\" is out of range for type real",
                            orig_num)));
        } else
            ereport(ERROR,
                (errcode(ERRCODE_INVALID_TEXT_REPRESENTATION),
                    errmsg("invalid input syntax for type %s: \"%s\"",
                        "real", orig_num)));
    }
#ifdef HAVE_BUGGY_SOLARIS_STRTOD
    else {
        /*
         * Many versions of Solaris have a bug wherein strtod sets endptr to
         * point one byte beyond the end of the string when given "inf" or
         * "infinity".
         */
        if (endptr != num && endptr[-1] == '\0')
            endptr--;
    }
#endif /* HAVE_BUGGY_SOLARIS_STRTOD */

    /* skip trailing whitespace */
    while (*endptr != '\0' && isspace((unsigned char)*endptr))
        endptr++;

    /* if there is any junk left at the end of the string, bail out */
    if (*endptr != '\0')
        ereport(ERROR,
            (errcode(ERRCODE_INVALID_TEXT_REPRESENTATION),
                errmsg("invalid input syntax for type %s: \"%s\"",
                    "real", orig_num)));

    /*
     * if we get here, we have a legal double, still need to check to see if
     * it's a legal float4
     */
    // CHECKFLOATVAL((float4) val, isinf(val), val == 0);

    return ((float4)val);
}
