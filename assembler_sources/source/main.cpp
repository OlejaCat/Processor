#include <stdio.h>

#include "assembler.h"
#include "logger.h"

int main(const int argc, const char** argv)
{
    openLogFile("log.txt");

    if (argc <= 1)
    {
        printf("No arguments given\n");
    }

    if (argc == 3)
    {
        assembleFile(argv[1], argv[2]);
    }

    return 0;
}
