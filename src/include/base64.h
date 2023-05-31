#pragma once

#define BASE64_PAD '='
#define BASE64DE_FIRST '+'
#define BASE64DE_LAST 'z'
#define BASE64_ENCODE_OUT_SIZE(s) ((unsigned int)((((s) + 2) / 3) * 4 + 1))
#define BASE64_DECODE_OUT_SIZE(s) ((unsigned int)(((s) / 4) * 3))

/* BASE 64 encode table */
static const char base64en[] = {
    'A',
    'B',
    'C',
    'D',
    'E',
    'F',
    'G',
    'H',
    'I',
    'J',
    'K',
    'L',
    'M',
    'N',
    'O',
    'P',
    'Q',
    'R',
    'S',
    'T',
    'U',
    'V',
    'W',
    'X',
    'Y',
    'Z',
    'a',
    'b',
    'c',
    'd',
    'e',
    'f',
    'g',
    'h',
    'i',
    'j',
    'k',
    'l',
    'm',
    'n',
    'o',
    'p',
    'q',
    'r',
    's',
    't',
    'u',
    'v',
    'w',
    'x',
    'y',
    'z',
    '0',
    '1',
    '2',
    '3',
    '4',
    '5',
    '6',
    '7',
    '8',
    '9',
    '+',
    '/',
};

/* ASCII order for BASE 64 decode, 255 in unused character */
static const unsigned char base64de[] = {
    /* nul, soh, stx, etx, eot, enq, ack, bel, */
    255, 255, 255, 255, 255, 255, 255, 255,

    /*  bs,  ht,  nl,  vt,  np,  cr,  so,  si, */
    255, 255, 255, 255, 255, 255, 255, 255,

    /* dle, dc1, dc2, dc3, dc4, nak, syn, etb, */
    255, 255, 255, 255, 255, 255, 255, 255,

    /* can,  em, sub, esc,  fs,  gs,  rs,  us, */
    255, 255, 255, 255, 255, 255, 255, 255,

    /*  sp, '!', '"', '#', '$', '%', '&', ''', */
    255, 255, 255, 255, 255, 255, 255, 255,

    /* '(', ')', '*', '+', ',', '-', '.', '/', */
    255, 255, 255, 62, 255, 255, 255, 63,

    /* '0', '1', '2', '3', '4', '5', '6', '7', */
    52, 53, 54, 55, 56, 57, 58, 59,

    /* '8', '9', ':', ';', '<', '=', '>', '?', */
    60, 61, 255, 255, 255, 255, 255, 255,

    /* '@', 'A', 'B', 'C', 'D', 'E', 'F', 'G', */
    255, 0, 1, 2, 3, 4, 5, 6,

    /* 'H', 'I', 'J', 'K', 'L', 'M', 'N', 'O', */
    7, 8, 9, 10, 11, 12, 13, 14,

    /* 'P', 'Q', 'R', 'S', 'T', 'U', 'V', 'W', */
    15, 16, 17, 18, 19, 20, 21, 22,

    /* 'X', 'Y', 'Z', '[', '\', ']', '^', '_', */
    23, 24, 25, 255, 255, 255, 255, 255,

    /* '`', 'a', 'b', 'c', 'd', 'e', 'f', 'g', */
    255, 26, 27, 28, 29, 30, 31, 32,

    /* 'h', 'i', 'j', 'k', 'l', 'm', 'n', 'o', */
    33, 34, 35, 36, 37, 38, 39, 40,

    /* 'p', 'q', 'r', 's', 't', 'u', 'v', 'w', */
    41, 42, 43, 44, 45, 46, 47, 48,

    /* 'x', 'y', 'z', '{', '|', '}', '~', del, */
    49, 50, 51, 255, 255, 255, 255, 255
};

unsigned int
base64_encode(const unsigned char* in, unsigned int inlen, char* out);

unsigned int
base64_decode(const char* in, unsigned int inlen, unsigned char* out);

void print_base64(const void* cipher, int length, int base64_length, char* message);