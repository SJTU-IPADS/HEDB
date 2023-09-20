#include <rr_utils.hpp>
#include <stdarg.h>
#include <string.h>

/*
 *  mode: 1 for record, 0 for replay
 *  args_num: the number of buffers need to be copied
 *  va_args: int, char *, int, char *, ... (int for length)
 */
void rrprintf(int mode, char* fix_buf, int args_num, ...)
{
    bool number;
    int length;
    int count;
    char* buffer;
    va_list arg;

    number = true;
    count = 0;
    va_start(arg, args_num);

    do {
        if (number) {
            length = va_arg(arg, size_t);
            number = false;
        } else {
            buffer = va_arg(arg, char*);
            number = true;
            count++;

            if (mode) {
                memcpy(fix_buf, buffer, length);
            } else {
                memcpy(buffer, fix_buf, length);
            }

            fix_buf += length;
        }
    } while (count != args_num);

    va_end(arg);
}