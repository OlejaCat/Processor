#include "processor.h"

#include <stdio.h>
#include <assert.h>

#include "stack.h"
#include "helpful_functions.h"
#include "../../command_processing/include/command_handler.h"


// static --------------------------------------------------------------------------------------------------------------

typedef int asm_type;

typedef struct SPU
{
    uint8_t* code;
    size_t   ip;
    size_t   size_of_code;
    Stack*   program_stack;
} SPU;

ProcessorErrorHandler readProgramCode(const char* path_to_program, SPU* spu);


// public --------------------------------------------------------------------------------------------------------------


ProcessorErrorHandler executeProgram(const char* path_to_program)
{
    assert(path_to_program != NULL);

    SPU spu = {};

    ProcessorErrorHandler return_code = readProgramCode(path_to_program, &spu);
    if (return_code != ProcessorErrorHandler_OK)
    {
        return ProcessorErrorHandler_ERROR;
    }


    spu.ip            = 0;
    spu.program_stack = stackCtor();

    while (spu.code[spu.ip] != MachineCommands_HLT)
    {
        switch(spu.code[spu.ip])
        {
            case MachineCommands_PUSH:
                asm_type push_argument =

                stackPush(spu.program_stack, stack_type item);
        }
    }

    stackDtor(spu.program_stack);

    return ProcessorErrorHandler_OK;
}


// static --------------------------------------------------------------------------------------------------------------

ProcessorErrorHandler pushArgument(SPU* spu)
{
    asm_type push_argument = 0;

    spu.
}


ProcessorErrorHandler readProgramCode(const char* path_to_program, SPU* spu)
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
