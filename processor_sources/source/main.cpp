#include <stdio.h>

#include "logger.h"
#include "processor.h"


int main(const int argc, const char** argv)
{ // NOTE сделать минимальный обработчик опция (типо --help)
    openLogFile("log.txt");

    if (argc <= 1)
    {
        printf("No arguments to compile");
        return 0;
    }

    executeProgram(argv[1]);

    return 0;
}
