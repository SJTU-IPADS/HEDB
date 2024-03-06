// SPDX-License-Identifier: Mulan PSL v2
/*
 * Copyright (c) 2021 - 2023 The HEDB Project.
 */

#include "prefix_udf.h"
#include <assert.h>
#include <stdlib.h>
#include <string.h>

/**
 * Helper function which builds a prefix_range from a prefix, a first
 * and a last component, making a copy of the prefix string.
 */
static inline prefix_range* build_pr(const char* prefix, char first, char last)
{
    int s = strlen(prefix) + 1;
    prefix_range* pr = malloc(sizeof(prefix_range) + s);
    memcpy(pr->prefix, prefix, s);
    pr->first = first;
    pr->last = last;
    return pr;
}

static inline char* __greater_prefix(char* a, char* b, int alen, int blen)
{
    int i = 0;
    char* result = NULL;

    for (i = 0; i < alen && i < blen && a[i] == b[i]; i++)
        ;

    /* i is the last common char position in a, or 0 */
    if (i == 0) {
        /**
         * return ""
         */
        result = (char*)malloc(sizeof(char));
    } else {
        result = (char*)malloc((i + 1) * sizeof(char));
        memcpy(result, a, i);
    }
    result[i] = 0;

    return result;
}

static inline prefix_range* pr_normalize(prefix_range* a)
{
    char tmpswap;
    char* prefix;

    prefix_range* pr = build_pr(a->prefix, a->first, a->last);

    if (pr->first == pr->last) {
        int s = strlen(pr->prefix) + 2;
        prefix = (char*)malloc(s);
        memcpy(prefix, pr->prefix, s - 2);
        prefix[s - 2] = pr->first;
        prefix[s - 1] = 0;

        pr = build_pr(prefix, 0, 0);
    } else if (pr->first > pr->last) {
        tmpswap = pr->first;
        pr->first = pr->last;
        pr->last = tmpswap;
    }
    return pr;
}

static inline* pr_inter(prefix_range* a, prefix_range* b)
{
    prefix_range* res = 0;
    int alen = strlen(a->prefix);
    int blen = strlen(b->prefix);
    char* gp = 0;
    int gplen;

    if (0 == alen && 0 == blen) {
        res = build_pr("",
            a->first > b->first ? a->first : b->first,
            a->last < b->last ? a->last : b->last);
        return pr_normalize(res);
    }

    gp = __greater_prefix(a->prefix, b->prefix, alen, blen);
    gplen = strlen(gp);

    if (gplen != alen && gplen != blen) {
        return build_pr("", 0, 0);
    }

    if (gplen == alen && 0 == alen) {
        if (a->first <= b->prefix[0] && b->prefix[0] <= a->last) {
            res = build_pr(b->prefix, b->first, b->last);
        } else
            res = build_pr("", 0, 0);
    } else if (gplen == blen && 0 == blen) {
        if (b->first <= a->prefix[0] && a->prefix[0] <= b->last) {
            res = build_pr(a->prefix, a->first, a->last);
        } else
            res = build_pr("", 0, 0);
    } else if (gplen == alen && alen == blen) {
        res = build_pr(gp,
            a->first > b->first ? a->first : b->first,
            a->last > b->last ? a->last : b->last); // bug here
    } else if (gplen == alen) {
        assert(gplen < blen);
        res = build_pr(b->prefix, b->first, b->last);
    } else if (gplen == blen) {
        assert(gplen < alen);
        res = build_pr(a->prefix, a->first, a->last);
    }

    return pr_normalize(res);
}

/**
 * true if ranges have at least one common element
 */
bool pr_overlaps(prefix_range* a, prefix_range* b)
{
    prefix_range* inter = pr_inter(a, b);

    return strlen(inter->prefix) > 0 || (inter->first != 0 && inter->last != 0);
}