#include <stdio.h>
#include <string.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include "common.h"

int main(int argc, char **argv)
{
    set_id();

    int fd = open("/proc/sys/vm/nr_hugepages", O_WRONLY);
    write(fd, argv[1], strlen(argv[1]));
    close(fd);
    return 0;
}
