/*
 * a simple UDF for timestamp
 */
#include "extension.hpp"
#include <enc_timestamp_ops.hpp>
#ifdef __cplusplus
extern "C" {
#endif
PG_FUNCTION_INFO_V1(pg_enc_timestamp_in);
PG_FUNCTION_INFO_V1(pg_enc_timestamp_out);
PG_FUNCTION_INFO_V1(pg_enc_timestamp_encrypt);
PG_FUNCTION_INFO_V1(pg_enc_timestamp_decrypt);
PG_FUNCTION_INFO_V1(pg_enc_timestamp_eq);
PG_FUNCTION_INFO_V1(pg_enc_timestamp_ne);
PG_FUNCTION_INFO_V1(pg_enc_timestamp_lt);
PG_FUNCTION_INFO_V1(pg_enc_timestamp_le);
PG_FUNCTION_INFO_V1(pg_enc_timestamp_gt);
PG_FUNCTION_INFO_V1(pg_enc_timestamp_ge);
PG_FUNCTION_INFO_V1(pg_enc_timestamp_cmp);
PG_FUNCTION_INFO_V1(date_part);
#ifdef __cplusplus
}
#endif

/* Convert a string to internal timestamp type. This function based on native postgres function 'timestamp_in'
 * @input: string as a postgres argument
`* @return: timestamp
*/
Timestamp pg_timestamp_in(char* str)
{

    Timestamp result;
    char workbuf[MAXDATELEN + MAXDATEFIELDS];
    char* field[MAXDATEFIELDS];
    int ftype[MAXDATEFIELDS];
    int dterr;
    int nf;
    int tz;
    int dtype;
    fsec_t fsec;
    struct pg_tm tt, *tm = &tt;
    // char buf[MAXDATELEN + 1];
    // char src_byte[TIMESTAMP_LENGTH];
    // int resp;

    dterr = ParseDateTime(str, workbuf, sizeof(workbuf), field, ftype, MAXDATEFIELDS, &nf);

    if (dterr == 0)
        dterr = DecodeDateTime(field, ftype, nf, &dtype, tm, &fsec, &tz);
    if (dterr != 0)
        DateTimeParseError(dterr, str, "timestamp");

    switch (dtype) {
    case DTK_DATE:
        if (tm2timestamp(tm, fsec, NULL, &result) != 0)
            ereport(ERROR,
                (errcode(ERRCODE_DATETIME_VALUE_OUT_OF_RANGE),
                    errmsg("timestamp out of range: \"%s\"", str)));
        break;

    case DTK_EPOCH:
        result = SetEpochTimestamp();
        break;

    case DTK_LATE:
        TIMESTAMP_NOEND(result);
        break;

    case DTK_EARLY:
        TIMESTAMP_NOBEGIN(result);
        break;

    default:
        elog(ERROR, "unexpected dtype %d while parsing timestamp \"%s\"",
            dtype, str);
        TIMESTAMP_NOEND(result);
    }

    return result;
}

/*
 * The function converts string to enc_timestamp. It is called by dbms every time it parses a query and finds an enc_timestamp element.
 * @input: string as a postgres arg
 * @return: enc_timestamp element as a string
 */
Datum pg_enc_timestamp_in(PG_FUNCTION_ARGS)
{
    char* pSrc = PG_GETARG_CSTRING(0);
    TIMESTAMP time;
    EncTimestamp* t = (EncTimestamp*)palloc0(ENC_TIMESTAMP_LENGTH);
    time = pg_timestamp_in(pSrc);
    // char ch[100];
    // sprintf(ch, "time is %lx", time);
    // print_info(ch);
    enc_timestamp_encrypt(&time, t);
    PG_RETURN_POINTER(t);
}
/*
 * The function converts enc_timestamp element to a string. If flag debugDecryption is true it decrypts the string and return unencrypted result.
 * @input: enc_timestamp element
 * @return: string
 */
Datum pg_enc_timestamp_out(PG_FUNCTION_ARGS)
{
    EncTimestamp* t = PG_GETARG_ENCTimestamp(0);
    TIMESTAMP timestamp;
    char* result = (char*)palloc0(TIMESTAMP_LENGTH * sizeof(char));
    struct pg_tm tt, *tm = &tt;
    fsec_t fsec;
    char buf[MAXDATELEN + 1];

    enc_timestamp_decrypt(t, &timestamp);

    if (timestamp2tm(timestamp, NULL, tm, &fsec, NULL, NULL) == 0)
        EncodeDateTime(tm, fsec, false, 0, NULL, 1, buf);
    else {
        ereport(ERROR,
            (errcode(ERRCODE_DATETIME_VALUE_OUT_OF_RANGE),
                errmsg("timestamp out of range")));
    }
    result = pstrdup(buf);
    PG_RETURN_CSTRING(result);
}

/*
 *  Gets a string as a timestamp element, encrypts it and return enc_timestamp element as a string.
 *   Converts the input string to a int64 element, encrypts one and return base64 encrypted result.
 *    @input: string
 *    @return: a string describing enc_timestamp element.
 */
Datum pg_enc_timestamp_encrypt(PG_FUNCTION_ARGS)
{
    char* arg = PG_GETARG_CSTRING(0);
#ifdef NOT_USED
    Oid typelem = PG_GETARG_OID(1);
#endif
    // int32 typmod = PG_GETARG_INT32(2);

    Timestamp time;
    EncTimestamp* t = (EncTimestamp*)palloc0(ENC_TIMESTAMP_LENGTH);

    time = pg_timestamp_in(arg);
    enc_timestamp_encrypt(&time, t);
    PG_RETURN_POINTER(t);
}

/*
 *  Gets a string as a enc_timestamp element, decrypts it and return timestamp element as a string.
 *  @input: enc_timestamp element
 *   @return: string
 */
Datum pg_enc_timestamp_decrypt(PG_FUNCTION_ARGS)
{
    EncTimestamp* t = PG_GETARG_ENCTimestamp(0);
    Timestamp timestamp;
    // int resp;
    char* result;
    struct pg_tm tt, *tm = &tt;
    fsec_t fsec;
    char buf[MAXDATELEN + 1];

    enc_timestamp_decrypt(t, &timestamp);

    if (timestamp2tm(timestamp, NULL, tm, &fsec, NULL, NULL) == 0)
        EncodeDateTime(tm, fsec, false, 0, NULL, 1, buf);
    else {
        ereport(ERROR,
            (errcode(ERRCODE_DATETIME_VALUE_OUT_OF_RANGE),
                errmsg("timestamp out of range")));
    }
    result = pstrdup(buf);
    PG_RETURN_CSTRING(result);
}

/*
 * The function checks if the first input enc_timestamp is equal to the second one.
 * It is called by binary operator '=' defined in sql extension.
 * @input: two enc_timestamp values
 * @return: true, if the first integer is equal to the second one.
 *       false, otherwise
 */
Datum pg_enc_timestamp_eq(PG_FUNCTION_ARGS)
{
    EncTimestamp* t1 = PG_GETARG_ENCTimestamp(0);
    EncTimestamp* t2 = PG_GETARG_ENCTimestamp(1);

    int ans = 0;
    enc_timestamp_cmp(t1, t2, &ans);

    PG_RETURN_BOOL((ans == 0) ? true : false);
}

/*
 * The function checks if the first input enc_timestamp is not equal to the second one.
 * It is called by binary operator '=' defined in sql extension.
 * @input: two enc_timestamp values
 * @return: true, if the first integer is equal to the second one.
 *       false, otherwise
 */
Datum pg_enc_timestamp_ne(PG_FUNCTION_ARGS)
{
    EncTimestamp* t1 = PG_GETARG_ENCTimestamp(0);
    EncTimestamp* t2 = PG_GETARG_ENCTimestamp(1);

    int ans = 0;
    enc_timestamp_cmp(t1, t2, &ans);

    PG_RETURN_BOOL((ans != 0) ? true : false);
}

/*
 * The function checks if the first input enc_timestamp is less to the second one.
 * It is called by binary operator '=' defined in sql extension.
 * @input: two enc_timestamp values
 * @return: true, if the first integer is equal to the second one.
 *       false, otherwise
 */
Datum pg_enc_timestamp_lt(PG_FUNCTION_ARGS)
{
    EncTimestamp* t1 = PG_GETARG_ENCTimestamp(0);
    EncTimestamp* t2 = PG_GETARG_ENCTimestamp(1);

    int ans = 0;
    enc_timestamp_cmp(t1, t2, &ans);

    PG_RETURN_BOOL((ans == -1) ? true : false);
}

/*
 * The function checks if the first input enc_timestamp is less or equal to the second one.
 * It is called by binary operator '=' defined in sql extension.
 * @input: two enc_timestamp values
 * @return: true, if the first integer is equal to the second one.
 *       false, otherwise
 */
Datum pg_enc_timestamp_le(PG_FUNCTION_ARGS)
{
    EncTimestamp* t1 = PG_GETARG_ENCTimestamp(0);
    EncTimestamp* t2 = PG_GETARG_ENCTimestamp(1);

    int ans = 0;
    enc_timestamp_cmp(t1, t2, &ans);

    PG_RETURN_BOOL((ans <= 0) ? true : false);
}

/*
 * The function checks if the first input enc_timestamp is greater to the second one.
 * It is called by binary operator '=' defined in sql extension.
 * @input: two enc_timestamp values
 * @return: true, if the first integer is equal to the second one.
 *       false, otherwise
 */
Datum pg_enc_timestamp_gt(PG_FUNCTION_ARGS)
{
    EncTimestamp* t1 = PG_GETARG_ENCTimestamp(0);
    EncTimestamp* t2 = PG_GETARG_ENCTimestamp(1);

    int ans = 0;
    enc_timestamp_cmp(t1, t2, &ans);
    PG_RETURN_BOOL((ans > 0) ? true : false);
}

/*
 * The function checks if the first input enc_timestamp is greater or equal to the second one.
 * It is called by binary operator '=' defined in sql extension.
 * @input: two enc_timestamp values
 * @return: true, if the first integer is equal to the second one.
 *       false, otherwise
 */
Datum pg_enc_timestamp_ge(PG_FUNCTION_ARGS)
{
    EncTimestamp* t1 = PG_GETARG_ENCTimestamp(0);
    EncTimestamp* t2 = PG_GETARG_ENCTimestamp(1);

    int ans = 0;
    enc_timestamp_cmp(t1, t2, &ans);
    PG_RETURN_BOOL((ans >= 0) ? true : false);
}

/*
 * The function compares two values. It is called mostly during index building.
 * It compares them as a timestamp (int64 elements) and return {-1,0,1} as a result.
 * @input: two enc_timestamp values
 * @return: -1, 0 ,1
 */
Datum pg_enc_timestamp_cmp(PG_FUNCTION_ARGS)
{
    EncTimestamp* t1 = PG_GETARG_ENCTimestamp(0);
    EncTimestamp* t2 = PG_GETARG_ENCTimestamp(1);

    int ans = 0;
    enc_timestamp_cmp(t1, t2, &ans);

    PG_RETURN_INT32(ans);
}

Datum date_part(PG_FUNCTION_ARGS)
{
    char* get = text_to_cstring(PG_GETARG_TEXT_P(0));
    if (strcmp(get, "year") != 0) {
        ereport(ERROR, (errmsg("Only date_part('year', enc_timestamp) is currently implemented.")));
    }
    EncTimestamp* timestamp = PG_GETARG_ENCTimestamp(1);
    EncInt* result = (EncInt*)palloc(sizeof(EncInt));

    enc_timestamp_extract_year(timestamp, result);
    PG_RETURN_POINTER(result);
}
