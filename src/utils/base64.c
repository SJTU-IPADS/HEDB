/*
    Base64 Implementation.
*/
#include <base64.h>

unsigned int
base64_encode(const unsigned char* in, unsigned int inlen, char* out)
{
    int s;
    unsigned int i;
    unsigned int j;
    unsigned char c;
    unsigned char l;

    s = 0;
    l = 0;
    for (i = j = 0; i < inlen; i++) {
        c = in[i];

        switch (s) {
        case 0:
            s = 1;
            out[j++] = base64en[(c >> 2) & 0x3F];
            break;
        case 1:
            s = 2;
            out[j++] = base64en[((l & 0x3) << 4) | ((c >> 4) & 0xF)];
            break;
        case 2:
            s = 0;
            out[j++] = base64en[((l & 0xF) << 2) | ((c >> 6) & 0x3)];
            out[j++] = base64en[c & 0x3F];
            break;
        }
        l = c;
    }

    switch (s) {
    case 1:
        out[j++] = base64en[(l & 0x3) << 4];
        out[j++] = BASE64_PAD;
        out[j++] = BASE64_PAD;
        break;
    case 2:
        out[j++] = base64en[(l & 0xF) << 2];
        out[j++] = BASE64_PAD;
        break;
    }

    out[j] = 0;

    return j;
}

unsigned int
base64_decode(const char* in, unsigned int inlen, unsigned char* out)
{
    unsigned int i;
    unsigned int j;
    unsigned char c;

    if (inlen & 0x3) {
        return 0;
    }

    for (i = j = 0; i < inlen; i++) {
        if (in[i] == BASE64_PAD) {
            break;
        }
        if (in[i] < BASE64DE_FIRST || in[i] > BASE64DE_LAST) {
            return 0;
        }

        c = base64de[(unsigned char)in[i]];
        if (c == 255) {
            return 0;
        }

        switch (i & 0x3) {
        case 0:
            out[j] = (c << 2) & 0xFF;
            break;
        case 1:
            out[j++] |= (c >> 4) & 0x3;
            out[j] = (c & 0xF) << 4;
            break;
        case 2:
            out[j++] |= (c >> 2) & 0xF;
            out[j] = (c & 0x3) << 6;
            break;
        case 3:
            out[j++] |= c;
            break;
        }
    }

    return j;
}

void print_base64(const void* cipher, int length, int base64_length, char* message)
{
    char base64_str[base64_length];
    base64_encode((const unsigned char*)cipher, length, base64_str);
    // print_info("%s length: %d, contents: %s", message, length, base64_str);
}
