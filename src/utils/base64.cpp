//
// copied from
// https://github.com/sqlite/sqlite/blob/master/ext/misc/base64.c
//

#ifndef deliberate_fall_through
/* Quiet some compilers about some of our intentional code. */
#if GCC_VERSION >= 7000000
#define deliberate_fall_through __attribute__((fallthrough));
#else
#define deliberate_fall_through
#endif
#endif

#define PC 0x80 /* pad character */
#define WS 0x81 /* whitespace */
#define ND 0x82 /* Not above or digit-value */
#define PAD_CHAR '='

/* Decoding table, ASCII (7-bit) value to base 64 digit value or other */
static const unsigned char b64DigitValues[128] = {
    /*                             HT LF VT  FF CR       */
    ND, ND, ND, ND, ND, ND, ND, ND, ND, WS, WS, WS, WS, WS, ND, ND,
    /*                                                US */
    ND, ND, ND, ND, ND, ND, ND, ND, ND, ND, ND, ND, ND, ND, ND, ND,
    /*sp                                  +            / */
    WS, ND, ND, ND, ND, ND, ND, ND, ND, ND, ND, 62, ND, ND, ND, 63,
    /* 0  1            5            9            =       */
    52, 53, 54, 55, 56, 57, 58, 59, 60, 61, ND, ND, ND, PC, ND, ND,
    /*    A                                            O */
    ND, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14,
    /* P                               Z                 */
    15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, ND, ND, ND, ND, ND,
    /*    a                                            o */
    ND, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40,
    /* p                               z                 */
    41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51, ND, ND, ND, ND, ND};

static const char b64Numerals[64 + 1] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

#define BX_DV_PROTO(c) \
    ((((unsigned char)(c)) < 0x80) ? (unsigned char)(b64DigitValues[(unsigned char)(c)]) : 0x80)
#define IS_BX_DIGIT(bdp) (((unsigned char)(bdp)) < 0x80)
#define IS_BX_WS(bdp) ((bdp) == WS)
#define IS_BX_PAD(bdp) ((bdp) == PC)
#define BX_NUMERAL(dv) (b64Numerals[(unsigned char)(dv)])
/* Width of base64 lines. Should be an integer multiple of 4. */
#define B64_DARK_MAX 72

/* Encode a byte buffer into base64 text with linefeeds appended to limit
** encoded group lengths to B64_DARK_MAX or to terminate the last group.
*/
char *toBase64(const unsigned char *pIn, int nbIn, char *pOut)
{
    int nCol = 0;
    while (nbIn >= 3)
    {
        /* Do the bit-shuffle, exploiting unsigned input to avoid masking. */
        pOut[0] = BX_NUMERAL(pIn[0] >> 2);
        pOut[1] = BX_NUMERAL(((pIn[0] << 4) | (pIn[1] >> 4)) & 0x3f);
        pOut[2] = BX_NUMERAL(((pIn[1] & 0xf) << 2) | (pIn[2] >> 6));
        pOut[3] = BX_NUMERAL(pIn[2] & 0x3f);
        pOut += 4;
        nbIn -= 3;
        pIn += 3;
        if ((nCol += 4) >= B64_DARK_MAX || nbIn <= 0)
        {
            nCol = 0;
        }
    }
    if (nbIn > 0)
    {
        signed char nco = nbIn + 1;
        int nbe;
        unsigned long qv = *pIn++;
        for (nbe = 1; nbe < 3; ++nbe)
        {
            qv <<= 8;
            if (nbe < nbIn)
                qv |= *pIn++;
        }
        for (nbe = 3; nbe >= 0; --nbe)
        {
            char ce = (nbe < nco) ? BX_NUMERAL((unsigned char)(qv & 0x3f)) : PAD_CHAR;
            qv >>= 6;
            pOut[nbe] = ce;
        }
        pOut += 4;
    }
    *pOut = 0;
    return pOut;
}

/* Skip over text which is not base64 numeral(s). */
static char *skipNonB64(char *s, int nc)
{
    char c;
    while (nc-- > 0 && (c = *s) && !IS_BX_DIGIT(BX_DV_PROTO(c)))
        ++s;
    return s;
}

/* Decode base64 text into a byte buffer. */
unsigned char *fromBase64(char *pIn, int ncIn, unsigned char *pOut)
{
    if (ncIn > 0 && pIn[ncIn - 1] == '\n')
        --ncIn;
    while (ncIn > 0 && *pIn != PAD_CHAR)
    {
        static signed char nboi[] = {0, 0, 1, 2, 3};
        char *pUse = skipNonB64(pIn, ncIn);
        unsigned long qv = 0L;
        int nti, nbo, nac;
        ncIn -= (pUse - pIn);
        pIn = pUse;
        nti = (ncIn > 4) ? 4 : ncIn;
        ncIn -= nti;
        nbo = nboi[nti];
        if (nbo == 0)
            break;
        for (nac = 0; nac < 4; ++nac)
        {
            char c = (nac < nti) ? *pIn++ : b64Numerals[0];
            unsigned char bdp = BX_DV_PROTO(c);
            switch (bdp)
            {
            case ND:
                /*  Treat dark non-digits as pad, but they terminate decode too. */
                ncIn = 0;
                deliberate_fall_through;
            case WS:
                /* Treat whitespace as pad and terminate this group.*/
                nti = nac;
                deliberate_fall_through;
            case PC:
                bdp = 0;
                --nbo;
                deliberate_fall_through;
            default: /* bdp is the digit value. */
                qv = qv << 6 | bdp;
                break;
            }
        }
        switch (nbo)
        {
        case 3:
            pOut[2] = (qv)&0xff;
        case 2:
            pOut[1] = (qv >> 8) & 0xff;
        case 1:
            pOut[0] = (qv >> 16) & 0xff;
        }
        pOut += nbo;
    }
    return pOut;
}
