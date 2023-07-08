#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

int main(int argc, char **argv)
{
    if (0 != setuid(0) || 0 != setgid(0)) {
        fprintf(stderr, "setuid/setgid failed.\n");
        exit(-1);
    }
    system("service postgresql restart");
}
