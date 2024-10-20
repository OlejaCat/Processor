#include "processor_commands.h"

#include <stdio.h>
#include <string.h>
#include <assert.h>


void pushArgument(SPU* spu)
{
    assert(spu != NULL);

    arguments_type argument = 0;

    memcpy(&argument, spu->code + spu->ip + 1, sizeof(arguments_type));
    spu->ip += sizeof(arguments_type);

    stackPush(spu->program_stack, argument);
}

void addCommand(SPU* spu)
{
    assert(spu != NULL);

    arguments_type first_element  = 0;
    arguments_type second_element = 0;

    stackPop(spu->program_stack, &first_element);
    stackPop(spu->program_stack, &second_element);

    arguments_type result = 0;
    result = first_element + second_element;

    stackPush(spu->program_stack, result);
}


void mulCommand(SPU* spu)
{
    assert(spu != NULL);

    arguments_type first_element  = 0;
    arguments_type second_element = 0;

    stackPop(spu->program_stack, &first_element);
    stackPop(spu->program_stack, &second_element);

    arguments_type result = 0;
    result = first_element * second_element;

    stackPush(spu->program_stack, result);
}


void divCommand(SPU* spu)
{
    assert(spu != NULL);

    arguments_type first_element  = 0;
    arguments_type second_element = 0;

    stackPop(spu->program_stack, &first_element);
    stackPop(spu->program_stack, &second_element);

    arguments_type result = 0;
    result = first_element / second_element;

    stackPush(spu->program_stack, result);
}


void supCommand(SPU* spu)
{
    assert(spu != NULL);

    arguments_type first_element  = 0;
    arguments_type second_element = 0;

    stackPop(spu->program_stack, &first_element);
    stackPop(spu->program_stack, &second_element);

    arguments_type result = 0;
    result = first_element - second_element;

    stackPush(spu->program_stack, result);
}


void outCommand(SPU* spu)
{
    assert(spu != NULL);

    arguments_type element = 0;

    stackPop(spu->program_stack, &element);

    printf("Program out: %d\n", element);
}


void inCommand(SPU* spu)
{
    assert(spu != NULL);

    arguments_type element = 0;

    printf("Enter argument: ");
    scanf("%d", &element);

    stackPush(spu->program_stack, element);
}


void hltCommand(SPU* spu)
{
    // добавить сброс дампа

    printf("Program end\n");
}



