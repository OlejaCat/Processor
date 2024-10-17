#include "assembler.h"

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <assert.h>

#include "helpful_functions.h"
#include "logger.h"
#include "command_handler.h"

// static --------------------------------------------------------------------------------------------------------------


typedef struct CompiledData
{
    uint8_t* data_pointer;
    size_t   size;
} CompiledData;

CompiledData convertFile(char* input_stream, const size_t size_of_input_data);

static const size_t SIZE_OF_BUFFER = 32;


// public --------------------------------------------------------------------------------------------------------------


AssemblerErrorHandler assembleFileToMachineCode(const char* input_file_path, const char* output_file_path)
{
    assert(input_file_path  != NULL);
    assert(output_file_path != NULL);

    FILE* input_file = fopen(input_file_path, "r");
    if (!input_file)
    {
        return AssemblerErrorHandler_ERROR;
    }

    size_t size_of_file = getFileSize(input_file);

    char* input_data = (char*)calloc(size_of_file, sizeof(char));
    if (!input_data)
    {
        FCLOSE_NULL(input_file);
        return AssemblerErrorHandler_ERROR;
    }

    size_t return_code = fread(input_data,
                               sizeof(input_data[0]),
                               size_of_file,
                               input_file);
    if (return_code != size_of_file)
    {
        FREE_NULL(input_data);
        return AssemblerErrorHandler_ERROR;
    }

    FCLOSE_NULL(input_file);

    CompiledData output_data_struct = convertFile(input_data, size_of_file);

    FREE_NULL(input_data);

    Log(LogLevel_INFO, "pointer to datad: %p size: %lu", output_data_struct.data_pointer, output_data_struct.size);

    FILE* output_file = fopen(output_file_path, "wb");
    if (!output_file)
    {
        FREE_NULL(output_data_struct.data_pointer);
        return AssemblerErrorHandler_ERROR;
    }

    size_t size_writed = fwrite(output_data_struct.data_pointer,
                                sizeof(output_data_struct.data_pointer[0]),
                                output_data_struct.size,
                                output_file);
    if (size_writed != output_data_struct.size)
    {
        FREE_NULL(output_data_struct.data_pointer);
        FCLOSE_NULL(output_file);
        return AssemblerErrorHandler_ERROR;
    }

    Log(LogLevel_INFO, "We can be hear if we writed");

    FREE_NULL(output_data_struct.data_pointer);
    FCLOSE_NULL(output_file);


    return AssemblerErrorHandler_OK;
}


// static --------------------------------------------------------------------------------------------------------------


CompiledData convertFile(char* input_stream, const size_t size_of_input_data)
{
    assert(input_stream != NULL);

    uint8_t* output_stream = (uint8_t*)calloc(size_of_input_data, sizeof(uint8_t));


    char* ptr_of_string = strtok(input_stream, "\n");

    printf("%s", ptr_of_string);
    size_t shift = 0;
    while (ptr_of_string)
    {
        Log(LogLevel_INFO, "Just for fun");

        char command_buffer[SIZE_OF_BUFFER] = {};
        int number_argument = 0;

        int read_arguments = sscanf(ptr_of_string, "%s %d", command_buffer, &number_argument);

        Log(LogLevel_INFO, "%d", read_arguments);

        if (command_buffer[0] == '\0')
        {
            continue;
        }

        MachineCommands returned_command = convertAsmToMachine(command_buffer);

        if (returned_command == MachineCommands_UNKNOWN)
        {
            printf("Unknown command in given asm file"); // Заглушка
            return {.data_pointer = output_stream, .size = 0};
        }

        output_stream[shift++] = (uint8_t)returned_command;

        if (read_arguments == 2)
        {
            Log(LogLevel_INFO, "Number read: %d", number_argument);

            memcpy(output_stream + shift, &number_argument, sizeof(number_argument));
            shift += sizeof(number_argument);
        }

        ptr_of_string = strtok(NULL, "\n");
    }

    CompiledData compiled_data = {
        .data_pointer = output_stream,
        .size         = shift,
    };

    Log(LogLevel_INFO, "We hear in convert function pointer: %p size: %lu", output_stream, shift);

    return compiled_data;
}


