#include "assembler.h"

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <assert.h>

#include "helpful_functions.h"
#include "command_handler.h"

#include "logger.h"

// static --------------------------------------------------------------------------------------------------------------


typedef struct Mark
{
    char* mark_name;
    int   code_pointer;
} Mark;
typedef struct Assembler
{
    const char* input_file_path;
    size_t      input_file_size;
    char*       input_data;

    const char* output_file_path;
    size_t      output_file_size;
    uint8_t*    output_data;

    size_t*     jmp_list;
    size_t      jmp_list_size;

    Mark*       mark_list;
    size_t      mark_list_size;
} Assembler;

static MachineCommands convertCommandToMachineCode(const char* const command);
static AssemblerErrorHandler convertFileToMachineCode(Assembler* const assembler_struct);
static AssemblerErrorHandler assemblerCtor(Assembler* const assembler_struct);
static AssemblerErrorHandler assemblerDtor(Assembler* const assembler_struct);

static const size_t SIZE_OF_BUFFER    = 256;
static const size_t SIZE_OF_MARK_LIST = 16;


// public --------------------------------------------------------------------------------------------------------------


AssemblerErrorHandler assembleFile(const char* input_file_path, const char* output_file_path)
{
    assert(input_file_path  != NULL);
    assert(output_file_path != NULL);

    Assembler assembler = {};

    assembler.input_file_path  = input_file_path;
    assembler.output_file_path = output_file_path;

    AssemblerErrorHandler error_ctor    = assemblerCtor(&assembler);

    AssemblerErrorHandler error_convert = convertFileToMachineCode(&assembler);

    AssemblerErrorHandler errorDtor     = assemblerDtor(&assembler);


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

    assembler_struct->input_file_size = getFileSize(input_file);

    assembler_struct->input_data = (char*)calloc(assembler_struct->input_file_size, sizeof(char));
    if (!assembler_struct->input_data)
    {
        FCLOSE_NULL(input_file);
        return AssemblerErrorHandler_ERROR;
    }

    size_t return_code = fread(assembler_struct->input_data,
                               sizeof(assembler_struct->input_data[0]),
                               assembler_struct->input_file_size,
                               input_file);
    if (return_code != assembler_struct->input_file_size)
    {
        FREE_NULL(assembler_struct->input_data);
        return AssemblerErrorHandler_ERROR;
    }

    FCLOSE_NULL(input_file);

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

    for (size_t current_mark = 0; current_mark < assembler_struct->mark_list_size; current_mark++)
    {
        FREE_NULL(assembler_struct->mark_list[current_mark].mark_name);
    }

    FREE_NULL(assembler_struct->mark_list);
    FREE_NULL(assembler_struct->jmp_list);

    return AssemblerErrorHandler_OK;
}


static MachineCommands convertCommandToMachineCode(const char* const command)
{
    assert(command != NULL);

#define RETURN_MACHINE_CODE_(command_given, to_compare, number, out) \
    if (!strncmp(command, to_compare, number)) return out;

    RETURN_MACHINE_CODE_(command, HLT_COMMAND,  strlen(HLT_COMMAND),  MachineCommands_HLT);
    RETURN_MACHINE_CODE_(command, PUSH_COMMAND, strlen(PUSH_COMMAND), MachineCommands_PUSH);
    RETURN_MACHINE_CODE_(command, ADD_COMMAND,  strlen(ADD_COMMAND),  MachineCommands_ADD);
    RETURN_MACHINE_CODE_(command, MUL_COMMAND,  strlen(MUL_COMMAND),  MachineCommands_MUL);
    RETURN_MACHINE_CODE_(command, SUB_COMMAND,  strlen(SUB_COMMAND),  MachineCommands_SUB);
    RETURN_MACHINE_CODE_(command, DIV_COMMAND,  strlen(DIV_COMMAND),  MachineCommands_DIV);
    RETURN_MACHINE_CODE_(command, OUT_COMMAND,  strlen(OUT_COMMAND),  MachineCommands_OUT);
    RETURN_MACHINE_CODE_(command, IN_COMMAND,   strlen(IN_COMMAND),   MachineCommands_IN);
    RETURN_MACHINE_CODE_(command, JMP_COMMAND,  strlen(JMP_COMMAND),  MachineCommands_JMP);
    RETURN_MACHINE_CODE_(command, JA_COMMAND,   strlen(JA_COMMAND),   MachineCommands_JA);
    RETURN_MACHINE_CODE_(command, JAE_COMMAND,  strlen(JAE_COMMAND),  MachineCommands_JAE);
    RETURN_MACHINE_CODE_(command, JB_COMMAND,   strlen(JB_COMMAND),   MachineCommands_JB);
    RETURN_MACHINE_CODE_(command, JBE_COMMAND,  strlen(JBE_COMMAND),  MachineCommands_JBE);
    RETURN_MACHINE_CODE_(command, JE_COMMAND,   strlen(JE_COMMAND),   MachineCommands_JE);
    RETURN_MACHINE_CODE_(command, JNE_COMMAND,  strlen(JNE_COMMAND),  MachineCommands_JNE);

#undef  RETURN_MACHINE_CODE_

    return MachineCommands_UNKNOWN;
}


static AssemblerErrorHandler convertFileToMachineCode(Assembler* const assembler_struct)
{
    assert(assembler_struct != NULL);

    size_t len = assembler_struct->input_file_size;
    while(len > 0 && assembler_struct->input_data[len - 1] == '\n')
    {
        len--;
        assembler_struct->input_data[len] = '\0';
    }

    // FIXME добавить динамическое выделение памяти
    assembler_struct->output_data = (uint8_t*)calloc(assembler_struct->input_file_size, sizeof(uint8_t));
    if (!assembler_struct->output_data)
    {
        return AssemblerErrorHandler_ERROR;
    }

    char* input_data_copy = (char*)calloc(assembler_struct->input_file_size, sizeof(char));
    if (!input_data_copy)
    {
        return AssemblerErrorHandler_ERROR;
    }
    memcpy(input_data_copy, assembler_struct->input_data, assembler_struct->input_file_size);

    assembler_struct->jmp_list = (size_t*)calloc(SIZE_OF_MARK_LIST, sizeof(size_t));
    if(!assembler_struct->jmp_list)
    {
        return AssemblerErrorHandler_ERROR;
    }
    assembler_struct->jmp_list_size = 0;

    assembler_struct->mark_list = (Mark*)calloc(SIZE_OF_MARK_LIST, sizeof(Mark));
    if (!assembler_struct->mark_list)
    {
        return AssemblerErrorHandler_ERROR;
    }
    assembler_struct->mark_list_size = 0;

    char* ptr_of_string = strtok(input_data_copy, "\n");

    assembler_struct->output_file_size = 0;
    do
    {
        char command_buffer[SIZE_OF_BUFFER] = {};
        char string_argument[SIZE_OF_BUFFER] = {};

        int read_arguments = sscanf(ptr_of_string, "%s %s", command_buffer, string_argument);

        if (command_buffer[0] == '\0')
        {
            continue;
        }

        char last_simbol = 0;
        char mark[SIZE_OF_BUFFER] = {};
        int mark_size = 0;

        sscanf(command_buffer, "%[^:]%n%c", mark, &mark_size, &last_simbol);

        if (last_simbol == ':')
        {
            assembler_struct->mark_list[assembler_struct->mark_list_size] = {
                .mark_name    = (char*)calloc((size_t)mark_size, sizeof(char*)),
                .code_pointer = (int)assembler_struct->output_file_size,
            };

            strcpy(assembler_struct->mark_list[assembler_struct->mark_list_size].mark_name, mark);

            assembler_struct->mark_list_size++;

            continue;
        }

        MachineCommands returned_command = convertCommandToMachineCode(command_buffer);

        Log(LogLevel_INFO, "command read: %d", returned_command);

        if (returned_command == MachineCommands_UNKNOWN)
        {
            printf("Unknown command in given asm file\n"); // Заглушка
            return AssemblerErrorHandler_ERROR;
        }

        assembler_struct->output_data[assembler_struct->output_file_size] = (uint8_t)returned_command;
        assembler_struct->output_file_size++;

        if (read_arguments == 2)
        {
            if (returned_command == MachineCommands_JMP
             || returned_command == MachineCommands_JA
             || returned_command == MachineCommands_JAE
             || returned_command == MachineCommands_JB
             || returned_command == MachineCommands_JBE
             || returned_command == MachineCommands_JE
             || returned_command == MachineCommands_JNE)
            {
                assembler_struct->jmp_list[assembler_struct->jmp_list_size] = assembler_struct->output_file_size;
                assembler_struct->jmp_list_size++;

                assembler_struct->output_file_size += sizeof(int); // FIXME заменить на typedef
            }
            else
            {
                int number_argument = 0;
                sscanf(string_argument, "%d", &number_argument);

                memcpy(assembler_struct->output_data + assembler_struct->output_file_size,
                       &number_argument,
                       sizeof(number_argument));
                assembler_struct->output_file_size += sizeof(number_argument);
            }
        }
    }
    while ((ptr_of_string = strtok(NULL, "\n")) != NULL);

    FREE_NULL(input_data_copy);

    // вторая часть

    size_t jmp_counter = 0;

    ptr_of_string = strtok(assembler_struct->input_data, "\n");
    do
    {
        char command_buffer[SIZE_OF_BUFFER] = {};
        char string_argument[SIZE_OF_BUFFER] = {};

        int read_arguments = sscanf(ptr_of_string, "%s %s", command_buffer, string_argument);

        MachineCommands returned_command = convertCommandToMachineCode(command_buffer);

        if (read_arguments == 2)
        {
            if (returned_command == MachineCommands_JMP
             || returned_command == MachineCommands_JA
             || returned_command == MachineCommands_JAE
             || returned_command == MachineCommands_JB
             || returned_command == MachineCommands_JBE
             || returned_command == MachineCommands_JE
             || returned_command == MachineCommands_JNE)
            {
                char last_simbol = 0;
                char jmp_mark[SIZE_OF_BUFFER] = {};

                sscanf(string_argument, "%[^:]%c", jmp_mark, &last_simbol);

                if (last_simbol != ':')
                {
                    return AssemblerErrorHandler_ERROR;
                }

                for (size_t current_mark = 0; current_mark < assembler_struct->mark_list_size; current_mark++)
                {
                    if (!strcmp(jmp_mark, assembler_struct->mark_list[current_mark].mark_name))
                    {
                        memcpy(assembler_struct->output_data + assembler_struct->jmp_list[jmp_counter],
                               &assembler_struct->mark_list[current_mark].code_pointer,
                               sizeof(int));

                        break;
                    }
                }

                jmp_counter++;
            }
        }
    }
    while ((ptr_of_string = strtok(NULL, "\n")) != NULL);

    return AssemblerErrorHandler_OK;
}
