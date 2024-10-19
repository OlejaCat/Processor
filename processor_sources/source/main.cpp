#include <stdio.h>

#include "processor.h"


int main(const int argc, const char** argv)
{
    if (argc <= 1)
    {
        printf("No arguments to compile");
        return 0;
    }

    if (argc == 2)
    {
        executeProgram(argv[1]);
    }

    return 0;
}
