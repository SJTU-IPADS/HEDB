#include <stdio.h>
#include <string.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include "common.h"

int main(int argc, char **argv)
{
    set_id();

    system("service postgresql restart");
    return 0;
}
