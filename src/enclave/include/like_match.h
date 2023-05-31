#pragma once
#define GETCHAR(t) (t)
#define LIKE_TRUE 1
#define LIKE_FALSE 0
#define LIKE_ABORT 0

#define CHAREQ(p1, p2) (*(p1) == *(p2))
#define NextByte(p, plen) ((p)++, (plen)--)
#define NextChar(p, plen) NextByte((p), (plen))
#define CopyAdvChar(dst, src, srclen) (*(dst)++ = *(src)++, (srclen)--)

#ifdef __cplusplus
extern "C" {
#endif

int MatchText(char*, int, char*, int);

#ifdef __cplusplus
}
#endif