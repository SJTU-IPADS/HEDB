#pragma once

#include "sync.h"
typedef enum ShmReqStatus {
    SHM_NONE,
    SHM_GET,
    SHM_FREE,
    SHM_DONE,
    SHM_EXIT
} ShmReqStat;

#define GET(a, n) ((a >> n) & 1U)
#define SET(a, n) { a |= 1UL << n; }
#define CLEAR(a, n) { a &= ~(1UL << n); }

typedef struct ops_server {
    int volatile lock;
    int __res; // reserved filed, avoid cache false sharing.
    ShmReqStat volatile status;
    int free_id; // free region id
    int ret_id; // return region id
    unsigned int bitmap;
} OpsServer;
