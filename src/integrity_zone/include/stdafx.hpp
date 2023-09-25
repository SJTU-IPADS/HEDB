#pragma once
#include <stdio.h>
#include <stdint.h>
#include <unistd.h>
#include <string.h>
#include <math.h>
#include <float.h>
#include <limits.h>

#ifdef __cplusplus
extern "C" {
#endif

#include <postgres.h>
#include <fmgr.h>
#include <funcapi.h>
#include <utils/array.h>
#include <utils/arrayaccess.h>
#include <utils/builtins.h>
#include <utils/numeric.h>
#include <datatype/timestamp.h>
#include <utils/datetime.h>
#include <utils/timestamp.h>
#include <c.h>

#ifdef __cplusplus
}

#define print_info(...) \
            ereport(INFO, (errmsg(__VA_ARGS__)))
#define print_error(...) \
            ereport(ERROR, (errmsg(__VA_ARGS__)))
#else

#define print_info(...) \
            printf(__VA_ARGS__)
#define print_error(...) \
            printf(__VA_ARGS__)

#endif

#include "defs.h"
