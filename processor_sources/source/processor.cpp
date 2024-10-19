#include "processor.h"

#include <stdio.h>
#include <assert.h>

#include "helpful_functions.h"
#include "command_handler.h"
#include "processor_commands.h"


// static --------------------------------------------------------------------------------------------------------------


static ProcessorErrorHandler readProgramCode(const char* path_to_program, SPU* spu);
static ProcessorErrorHandler spuInit(const char* path_to_program, SPU* spu);


// public --------------------------------------------------------------------------------------------------------------


ProcessorErrorHandler executeProgram(const char* path_to_program)
{
    assert(path_to_program != NULL);

    SPU spu = {};
    spuInit(path_to_program, &spu);

    while (spu.code[spu.ip] != MachineCommands_HLT)
    {
        switch(spu.code[spu.ip])
        {
            case MachineCommands_PUSH:
                pushArgument(&spu);

            case MachineCommands_ADD:
                addCommand(&spu);

            case MachineCommands_MUL:
                mulCommand(&spu);

            case MachineCommands_DIV:
                divCommand(&spu);

            case MachineCommands_SUB:
                supCommand(&spu);

            default:
                assert(0 && "Unknown command");
        }
    }

    stackDtor(spu.program_stack);

    return ProcessorErrorHandler_OK;
}


// static --------------------------------------------------------------------------------------------------------------


static ProcessorErrorHandler spuInit(const char* path_to_program, SPU* spu)
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

}


static ProcessorErrorHandler readProgramCode(const char* path_to_program, SPU* spu)
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
