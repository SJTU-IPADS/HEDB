//
// copied from
// https://raw.githubusercontent.com/luvitrocks/luvit-base64/master/base64.c
//

// fastest encoding variation I've come up with (Not really, see comments after the article!)
// copied from
// http://www.experts-exchange.com/Programming/System/Windows__Programming/3_216-Fast-Base64-Encode-and-Decode.html
//

#include <assert.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

//----------------------------------------------------
// Using two-unsigned char lookup table
// must call here before calling the above
//----------------------------------------------------
static char Base64Digits[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
unsigned short Base64Digits8192[4096];

unsigned short* gpLookup16 = 0;
static unsigned char LookupDigits[] = {
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, // gap: ctrl chars
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, // gap: ctrl chars
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, // gap: spc,!"#$%'()*
    62, // +
    0, 0, 0, // gap ,-.
    63, // /
    52, 53, 54, 55, 56, 57, 58, 59, 60, 61, // 0-9
    0, 0, 0, // gap: :;<
    99, //  = (end padding)
    0, 0, 0, // gap: >?@
    0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16,
    17, 18, 19, 20, 21, 22, 23, 24, 25, // A-Z
    0, 0, 0, 0, 0, 0, // gap: [\]^_`
    26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40, 41, 42,
    43, 44, 45, 46, 47, 48, 49, 50, 51, // a-z
    0, 0, 0, 0, // gap: {|}~ (and the rest...)
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
};

void SetupLookup16()
{
    int nLenTbl = 256 * 256; // yes, the table is 128Kb!
    if (NULL == gpLookup16) {
        gpLookup16 = new unsigned short[nLenTbl];
    }
    unsigned short* p = gpLookup16;
    for (int j = 0; j < 256; j++) {
        for (int k = 0; k < 256; k++) {
            unsigned short w;
            w = LookupDigits[k] << 8;
            w |= LookupDigits[j] << 2; // pre-shifted! See notes
            *p++ = w;
        }
    }
}

static void
SetupTable8192()
{
    int j, k;
    for (j = 0; j < 64; j++) {
        for (k = 0; k < 64; k++) {
            unsigned short w;
            w = Base64Digits[k] << 8;
            w |= Base64Digits[j];
            Base64Digits8192[(j * 64) + k] = w;
        }
    }
}
// Utra-Fast base64-decoding, using 128KB lookup table
int FromBase64Fast(const unsigned char* pSrc, int nLenSrc, unsigned char* pDst, int nLenDst)
{
    if (NULL == gpLookup16)
        SetupLookup16(); // see below
    int nLenOut = 0;
    if (nLenDst < ((nLenSrc / 4) - 1) * 3) {
        return (0); // (buffer too small)
    }
    int nLoopMax = (nLenSrc / 4) - 1;
    unsigned short* pwSrc = (unsigned short*)pSrc;
    for (int j = 0; j < nLoopMax; j++) {
        unsigned short s1 = gpLookup16[pwSrc[0]]; // translate two "digits" at once
        unsigned short s2 = gpLookup16[pwSrc[1]]; // ... and two more

        unsigned long n32;
        n32 = s1; // xxxxxxxx xxxxxxxx xx111111 222222xx
        n32 <<= 10; // xxxxxxxx 11111122 2222xxxx xxxxxxxx
        n32 |= s2 >> 2; // xxxxxxxx 11111122 22223333 33444444

        unsigned char b3 = (n32 & 0x00ff);
        n32 >>= 8; // in reverse (unsigned short order)
        unsigned char b2 = (n32 & 0x00ff);
        n32 >>= 8;
        unsigned char b1 = (n32 & 0x00ff);

        // *pDst++ = b1;  *pDst++ = b2;  *pDst++ = b3;  //slighly slower

        pDst[0] = b1; // slightly faster
        pDst[1] = b2;
        pDst[2] = b3;

        pwSrc += 2;
        pDst += 3;
    }
    nLenOut = ((nLenSrc / 4) - 1) * 3;

    //-------------------- special handling outside of loop for end
    unsigned short s1 = gpLookup16[pwSrc[0]];
    unsigned short s2 = gpLookup16[pwSrc[1]];

    unsigned long n32;
    n32 = s1;
    n32 <<= 10;
    n32 |= s2 >> 2;

    unsigned char b3 = (n32 & 0x00ff);
    n32 >>= 8;
    unsigned char b2 = (n32 & 0x00ff);
    n32 >>= 8;
    unsigned char b1 = (n32 & 0x00ff);

    // add that code to fix the length error
    // when the encoded string ends on '=' or '==' the final length should be
    // decrement
    unsigned char bb2 = pSrc[nLenSrc - 2];
    unsigned char bb3 = pSrc[nLenSrc - 1];

    if (nLenOut >= nLenDst)
        return (0); // error
    *pDst++ = b1;
    nLenOut++;

    if (bb2 != 61) {
        if (nLenOut >= nLenDst)
            return (0); // error
        *pDst++ = b2;
        nLenOut++;
    }
    if (bb3 != 61) {
        if (nLenOut >= nLenDst)
            return (0); // error
        *pDst++ = b3;
        nLenOut++;
    }

    return (nLenOut);
}

// Utra-Fast base64-decoding, using 128KB lookup table
int FromBase64Fast(const unsigned char* pSrc, int nLenSrc, char* pDst, int nLenDst)
{
    if (NULL == gpLookup16)
        SetupLookup16(); // see below
    int nLenOut = 0;
    if (nLenDst < ((nLenSrc / 4) - 1) * 3) {
        return (0); // (buffer too small)
    }
    int nLoopMax = (nLenSrc / 4) - 1;
    unsigned short* pwSrc = (unsigned short*)pSrc;
    for (int j = 0; j < nLoopMax; j++) {
        unsigned short s1 = gpLookup16[pwSrc[0]]; // translate two "digits" at once
        unsigned short s2 = gpLookup16[pwSrc[1]]; // ... and two more

        unsigned long n32;
        n32 = s1; // xxxxxxxx xxxxxxxx xx111111 222222xx
        n32 <<= 10; // xxxxxxxx 11111122 2222xxxx xxxxxxxx
        n32 |= s2 >> 2; // xxxxxxxx 11111122 22223333 33444444

        unsigned char b3 = (n32 & 0x00ff);
        n32 >>= 8; // in reverse (unsigned short order)
        unsigned char b2 = (n32 & 0x00ff);
        n32 >>= 8;
        unsigned char b1 = (n32 & 0x00ff);

        // *pDst++ = b1;  *pDst++ = b2;  *pDst++ = b3;  //slighly slower

        pDst[0] = b1; // slightly faster
        pDst[1] = b2;
        pDst[2] = b3;

        pwSrc += 2;
        pDst += 3;
    }
    nLenOut = ((nLenSrc / 4) - 1) * 3;

    //-------------------- special handling outside of loop for end
    unsigned short s1 = gpLookup16[pwSrc[0]];
    unsigned short s2 = gpLookup16[pwSrc[1]];

    unsigned long n32;
    n32 = s1;
    n32 <<= 10;
    n32 |= s2 >> 2;

    unsigned char b3 = (n32 & 0x00ff);
    n32 >>= 8;
    unsigned char b2 = (n32 & 0x00ff);
    n32 >>= 8;
    unsigned char b1 = (n32 & 0x00ff);

    // add that code to fix the length error
    // when the encoded string ends on '=' or '==' the final length should be
    // decrement
    unsigned char bb2 = pSrc[nLenSrc - 2];
    unsigned char bb3 = pSrc[nLenSrc - 1];

    if (nLenOut >= nLenDst)
        return (0); // error
    *pDst++ = b1;
    nLenOut++;

    if (bb2 != 61) {
        if (nLenOut >= nLenDst)
            return (0); // error
        *pDst++ = b2;
        nLenOut++;
    }
    if (bb3 != 61) {
        if (nLenOut >= nLenDst)
            return (0); // error
        *pDst++ = b3;
        nLenOut++;
    }

    return (nLenOut);
}

int ToBase64Fast(const unsigned char* pSrc, int nLenSrc, char* pDst, int nLenDst)
{
    SetupTable8192();

    int nLenOut = ((nLenSrc + 2) / 3) * 4; // 4 out for every 3 in, rounded up
    if (nLenOut + 1 > nLenDst) {
        return (0); // fail!
    }

    unsigned short* pwDst = (unsigned short*)pDst;
    while (nLenSrc > 2) {
        unsigned int n = pSrc[0]; // xxx1
        n <<= 8; // xx1x
        n |= pSrc[1]; // xx12
        n <<= 8; // x12x
        n |= pSrc[2]; // x123

        pwDst[0] = Base64Digits8192[n >> 12];
        pwDst[1] = Base64Digits8192[n & 0x00000fff];

        nLenSrc -= 3;
        pwDst += 2;
        pSrc += 3;
    }
    // -------------- end of buffer special handling (see text)
    pDst = (char*)pwDst;

    if (nLenSrc > 0) { // some left after last triple
        int n1 = (*pSrc & 0xfc) >> 2;
        int n2 = (*pSrc & 0x03) << 4;
        if (nLenSrc > 1) { // corrected.  Thanks to jprichey
            pSrc++;
            n2 |= (*pSrc & 0xf0) >> 4;
        }
        *pDst++ = Base64Digits[n1]; // encode at least 2 outputs
        *pDst++ = Base64Digits[n2];
        if (nLenSrc == 2) { // 2 src unsigned chars left to encode, output xxx=
            int n3 = (*pSrc & 0x0f) << 2;
            pSrc++;
            n3 |= (*pSrc & 0xc0) >> 6;
            *pDst++ = Base64Digits[n3];
        }
        if (nLenSrc == 1) { // 1 src unsigned char left to encode, output xx==
            *pDst++ = '=';
        }
        *pDst++ = '=';
    }
    // *pDst= 0; nLenOut++ // could terminate with NULL, here
    return (nLenOut);
}
