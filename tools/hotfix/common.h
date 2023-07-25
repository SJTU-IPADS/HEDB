#include <unistd.h>
#include <stdlib.h>

static void set_id()
{
    if (0 != setuid(0) || 0 != setgid(0)) {
        fprintf(stderr, "setuid/setgid failed.\n");
        exit(-1);
    }
}
