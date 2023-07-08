#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

int main(int argc, char **argv)
{
    int fd = open("/proc/sys/vm/nr_hugepages", O_WRONLY);

    if (0 != setuid(0) || 0 != setgid(0)) {
        fprintf(stderr, "setuid/setgid failed.\n");
        exit(-1);
    }
    write(fd, argv[1], strlen(argv[1]));
    close(fd);
}
