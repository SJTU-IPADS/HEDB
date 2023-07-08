#pragma once
#include <stdbool.h>

typedef struct {
    char first;
    char last;
    char prefix[1]; /* this is a varlena structure, data follows */
} prefix_range;

#define CMD_PREFIX_OVERLAP 1001
bool pr_overlaps(prefix_range* a, prefix_range* b);