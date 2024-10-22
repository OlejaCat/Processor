#include "processor.h"

#include <stdio.h>
#include <string.h>
#include <assert.h>

#include "helpful_functions.h"
#include "command_handler.h"
#include "stack.h"


// static --------------------------------------------------------------------------------------------------------------


typedef int arguments_type;

typedef struct SPU
{
    uint8_t* code;
    size_t   ip;
    size_t   size_of_code;
    Stack*   program_stack;
    bool     end_flag;
} SPU;

static ProcessorErrorHandler readProgramCode(const char* path_to_program, SPU* const spu);
static ProcessorErrorHandler spuInit(const char* path_to_program, SPU* const spu);
static ProcessorErrorHandler spuDtor(SPU* const spu);

static void processMachineCode(SPU* const spu);

static void pushArgument(SPU* const spu);
static void addCommand(SPU* const spu);
static void mulCommand(SPU* const spu);
static void supCommand(SPU* const spu);
static void divCommand(SPU* const spu);
static void outCommand(SPU* const spu);
static void inCommand(SPU* const spu);
static void hltCommand(SPU* const spu);


// public --------------------------------------------------------------------------------------------------------------


ProcessorErrorHandler executeProgram(const char* path_to_program)
{
    assert(path_to_program != NULL);

    SPU spu = {};
    spuInit(path_to_program, &spu);

    processMachineCode(&spu);

    spuDtor(&spu);

    return ProcessorErrorHandler_OK;
}


// static --------------------------------------------------------------------------------------------------------------


static void processMachineCode(SPU* const spu)
{
    while (spu->end_flag)
    {
        switch(spu->code[spu->ip])
        {
            case MachineCommands_PUSH: pushArgument(spu);
                                       break;

            case MachineCommands_ADD:  addCommand(spu);
                                       break;

            case MachineCommands_MUL:  mulCommand(spu);
                                       break;

            case MachineCommands_DIV:  divCommand(spu);
                                       break;

            case MachineCommands_SUB:  supCommand(spu);
                                       break;

            case MachineCommands_OUT:  outCommand(spu);
                                       break;

            case MachineCommands_IN:   inCommand(spu);
                                       break;

            case MachineCommands_HLT:  hltCommand(spu);
                                       break;

            default:                   assert(0 && "Unknown command");
        }
        spu->ip++;
    }
}


static ProcessorErrorHandler spuInit(const char* path_to_program, SPU* const spu)
{
    assert(path_to_program != NULL);
    assert(spu             != NULL);

    ProcessorErrorHandler return_code = readProgramCode(path_to_program, spu);
    if (return_code != ProcessorErrorHandler_OK)
    {
        return ProcessorErrorHandler_ERROR;
    }

    spu->ip            = 0;
    spu->program_stack = stackCtor();
    spu->end_flag      = true;

    return ProcessorErrorHandler_OK;
}


static ProcessorErrorHandler spuDtor(SPU* spu)
{
    free(spu->code);
    stackDtor(spu->program_stack);

    memset(spu, 0, sizeof(SPU));

    return ProcessorErrorHandler_OK;
}


static ProcessorErrorHandler readProgramCode(const char* path_to_program, SPU* const spu)
{
    assert(path_to_program != NULL);
    assert(spu             != NULL);

    FILE* program_file = fopen(path_to_program, "rb");
    if (!program_file)
    {
        return ProcessorErrorHandler_OPEN_FILE_ERROR;
    }

    size_t size_of_file = getFileSize(program_file);

    uint8_t* program_code = (uint8_t*)calloc(size_of_file, sizeof(uint8_t));
    if (!program_code)
    {
        FCLOSE_NULL(program_file);
        return ProcessorErrorHandler_ERROR;
    }

    size_t return_code = fread(program_code,
                               sizeof(program_code[0]),
                               size_of_file,
                               program_file);
    if (return_code != size_of_file)
    {
        FREE_NULL(program_code);
        FCLOSE_NULL(program_file);
        return ProcessorErrorHandler_ERROR;
    }

    spu->code         = program_code;
    spu->size_of_code = size_of_file;

    return ProcessorErrorHandler_OK;
}


static void pushArgument(SPU* const spu)
{
    assert(spu != NULL);

    arguments_type argument = 0;

    memcpy(&argument, spu->code + spu->ip + 1, sizeof(arguments_type));
    spu->ip += sizeof(arguments_type);

    stackPush(spu->program_stack, argument);
}

static void addCommand(SPU* const spu)
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


static void mulCommand(SPU* const spu)
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


static void divCommand(SPU* const spu)
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


static void supCommand(SPU* const spu)
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


static void outCommand(SPU* const spu)
{
    assert(spu != NULL);

    arguments_type element = 0;

    stackPop(spu->program_stack, &element);

    printf("Program out: %d\n", element);
}


static void inCommand(SPU* const spu)
{
    assert(spu != NULL);

    arguments_type element = 0;

    printf("Enter argument: ");
    scanf("%d", &element);

    stackPush(spu->program_stack, element);
}


static void hltCommand(SPU* const spu)
{
    spu->end_flag = false;

    // добавить сброс дампа

    printf("Program end\n");
}
