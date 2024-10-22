#include "assembler.h"

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <assert.h>

#include "helpful_functions.h"
#include "command_handler.h"

// static --------------------------------------------------------------------------------------------------------------


typedef struct AssembledData
{
    uint8_t* data_pointer;
    size_t   size;
} AssembledData;

typedef struct Assembler
{
    const char* input_file_path;
    size_t      input_file_size;
    char*       input_data;

    const char* output_file_path;
    size_t      output_file_size;
    uint8_t*    output_data;
} Assembler;

static MachineCommands convertCommandToMachineCode(const char* const command);
static AssemblerErrorHandler convertFileToMachineCode(Assembler* const assembler_struct);
static AssemblerErrorHandler assemblerCtor(Assembler* const assembler_struct);
static AssemblerErrorHandler assemblerDtor(Assembler* const assembler_struct);

static const size_t SIZE_OF_BUFFER = 32;


// public --------------------------------------------------------------------------------------------------------------


AssemblerErrorHandler assembleFile(const char* input_file_path, const char* output_file_path)
{
    assert(input_file_path  != NULL);
    assert(output_file_path != NULL);

    Assembler assembler = {};

    assembler.input_file_path  = input_file_path;
    assembler.output_file_path = output_file_path;

    assemblerCtor(&assembler);

    convertFileToMachineCode(&assembler);

    assemblerDtor(&assembler);


    return AssemblerErrorHandler_OK;
}


// static --------------------------------------------------------------------------------------------------------------


static AssemblerErrorHandler assemblerCtor(Assembler* const assembler_struct)
{
    assert(assembler_struct != NULL);

    FILE* input_file = fopen(assembler_struct->input_file_path, "r");
    if (!input_file)
    {
        return AssemblerErrorHandler_ERROR;
    }

    size_t input_file_size = getFileSize(input_file);

    char* input_data = (char*)calloc(input_file_size, sizeof(char));
    if (!input_data)
    {
        FCLOSE_NULL(input_file);
        return AssemblerErrorHandler_ERROR;
    }

    size_t return_code = fread(input_data,
                               sizeof(input_data[0]),
                               input_file_size,
                               input_file);
    if (return_code != input_file_size)
    {
        FREE_NULL(input_data);
        return AssemblerErrorHandler_ERROR;
    }

    FCLOSE_NULL(input_file);

    assembler_struct->input_data      = input_data;
    assembler_struct->input_file_size = input_file_size;

    return AssemblerErrorHandler_OK;
}

static AssemblerErrorHandler assemblerDtor(Assembler* const assembler_struct)
{
    assert(assembler_struct != NULL);

    FREE_NULL(assembler_struct->input_data);

    FILE* output_file = fopen(assembler_struct->output_file_path, "wb");
    if (!output_file)
    {
        FREE_NULL(assembler_struct->output_data);
        return AssemblerErrorHandler_ERROR;
    }

    size_t size_writed = fwrite(assembler_struct->output_data,
                                sizeof(assembler_struct->output_data[0]),
                                assembler_struct->output_file_size,
                                output_file);
    if (size_writed != assembler_struct->output_file_size)
    {
        FREE_NULL(assembler_struct->output_data);
        FCLOSE_NULL(output_file);
        return AssemblerErrorHandler_ERROR;
    }

    FREE_NULL(assembler_struct->output_data);
    FCLOSE_NULL(output_file);

    return AssemblerErrorHandler_OK;
}


static MachineCommands convertCommandToMachineCode(const char* const command)
{
    assert(command != NULL);

#define RETURN_MACHINE_CODE_(command_given, to_compare, number, out) \
    if (!strncmp(command, to_compare, number)) return out;

    RETURN_MACHINE_CODE_(command, PUSH_COMMAND, strlen(PUSH_COMMAND), MachineCommands_PUSH);
    RETURN_MACHINE_CODE_(command, ADD_COMMAND,  strlen(ADD_COMMAND),  MachineCommands_ADD);
    RETURN_MACHINE_CODE_(command, MUL_COMMAND,  strlen(MUL_COMMAND),  MachineCommands_MUL);
    RETURN_MACHINE_CODE_(command, SUB_COMMAND,  strlen(SUB_COMMAND),  MachineCommands_SUB);
    RETURN_MACHINE_CODE_(command, DIV_COMMAND,  strlen(DIV_COMMAND),  MachineCommands_DIV);
    RETURN_MACHINE_CODE_(command, OUT_COMMAND,  strlen(OUT_COMMAND),  MachineCommands_OUT);
    RETURN_MACHINE_CODE_(command, IN_COMMAND,   strlen(IN_COMMAND),   MachineCommands_IN);
    RETURN_MACHINE_CODE_(command, HLT_COMMAND,  strlen(HLT_COMMAND),  MachineCommands_HLT);

#undef  RETURN_MACHINE_CODE_

    return MachineCommands_UNKNOWN;
}


static AssemblerErrorHandler convertFileToMachineCode(Assembler* const assembler_struct)
{
    assert(assembler_struct != NULL);

    uint8_t* output_data = (uint8_t*)calloc(assembler_struct->input_file_size, sizeof(uint8_t));
    if (!output_data)
    {
        return AssemblerErrorHandler_ERROR;
    }

    size_t len = assembler_struct->input_file_size;
    while(len > 0 && assembler_struct->input_data[len - 1] == '\n')
    {
        len--;
        assembler_struct->input_data[len] = '\0';
    }

    // NOTE mb do while
    char* ptr_of_string = strtok(assembler_struct->input_data, "\n");

    size_t shift = 0;
    do
    {
        char command_buffer[SIZE_OF_BUFFER] = {};
        int number_argument = 0;

        int read_arguments = sscanf(ptr_of_string, "%s %d", command_buffer, &number_argument);

        if (command_buffer[0] == '\0')
        {
            continue;
        }

        MachineCommands returned_command = convertCommandToMachineCode(command_buffer);

        if (returned_command == MachineCommands_UNKNOWN)
        {
            printf("Unknown command in given asm file"); // Заглушка
            return AssemblerErrorHandler_ERROR;
        }

        output_data[shift] = (uint8_t)returned_command;
        shift++;

        if (read_arguments == 2)
        {
            memcpy(output_data + shift, &number_argument, sizeof(number_argument));
            shift += sizeof(number_argument);
        }

    }
    while ((ptr_of_string = strtok(NULL, "\n")) != NULL);

    assembler_struct->output_data      = output_data;
    assembler_struct->output_file_size = shift;

    return AssemblerErrorHandler_OK;
}


