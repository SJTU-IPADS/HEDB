#include <cctype>
#include <cstdio>
#include <cstring>

void _print_hex(const char* what, const void* v, const unsigned long l)
{
    char tmp[500];
    const unsigned char* p = (const unsigned char*)v;
    unsigned long x, y = 0, z;
    sprintf(tmp, "%s contents: \n", what);
    for (x = 0; x < l;) {
        sprintf(tmp + strlen(tmp), "%02x ", p[x]);
        if (!(++x % 16) || x == l) {
            if ((x % 16) != 0) {
                z = 16 - (x % 16);
                if (z >= 8)
                    sprintf(tmp + strlen(tmp), " ");
                for (; z != 0; --z) {
                    sprintf(tmp + strlen(tmp), "   ");
                }
            }
            sprintf(tmp + strlen(tmp), " | ");
            for (; y < x; y++) {
                if ((y % 8) == 0)
                    sprintf(tmp + strlen(tmp), " ");
                if (isgraph(p[y]))
                    sprintf(tmp + strlen(tmp), "%c", p[y]);
                else
                    sprintf(tmp + strlen(tmp), ".");
            }
            sprintf(tmp + strlen(tmp), "\n");
        } else if ((x % 8) == 0) {
            sprintf(tmp + strlen(tmp), " ");
        }
    }
    // print_info("%s\n", tmp);
}
