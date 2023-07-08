#pragma once
#include <defs.h>
#include <stdafx.hpp>
#include <enc_types.h>

//variable length text.  this type is for storage 
// 4 byte length + flexible length.
typedef struct varlena EncText;
typedef struct {
    int32_t size;
    char data[FLEXIBLE_ARRAY_MEMBER];
} lentext;

#define ORDER_NONE -1

#define DatumGetEncInt(X)  ((EncInt *) DatumGetPointer(X))
#define PG_GETARG_ENCINT(n) DatumGetEncInt(PG_GETARG_DATUM(n))

#define DatumGetEncFloat(X)  ((EncFloat *) DatumGetPointer(X))
#define PG_GETARG_ENCFlOAT(n) DatumGetEncFloat(PG_GETARG_DATUM(n))

#define DatumGetEncTimestamp(X)  ((EncTimestamp *) DatumGetPointer(X))
#define PG_GETARG_ENCTimestamp(n) DatumGetEncTimestamp(PG_GETARG_DATUM(n))

// #define DatumGetEncStr(X)  ((EncStr *) DatumGetPointer(X))
#define PG_GETARG_ENCTEXT_P(n) ((EncText *)PG_DETOAST_DATUM(PG_GETARG_DATUM(n)))

#ifndef PG_FUNCTION_ARGS	
#define PG_FUNCTION_ARGS FunctionCallInfo fcinfo
#endif

