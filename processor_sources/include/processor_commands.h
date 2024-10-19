#ifndef PROCESSOR_COMMANDS_H
#define PROCESSOR_COMMANDS_H

#include <stdint.h>
#include <stdlib.h>
#include <stack.h>

typedef int arguments_type;

typedef struct SPU
{
    uint8_t* code;
    size_t   ip;
    size_t   size_of_code;
    Stack*   program_stack;
} SPU;

void pushArgument(SPU* spu);
void addCommand(SPU* spu);
void mulCommand(SPU* spu);
void supCommand(SPU* spu);
void divCommand(SPU* spu);

#endif // PROCESSOR_COMMANDS_H
