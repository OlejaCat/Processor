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

typedef int arguments_type;

typedef struct Mark
{
    char*          mark_name;
    arguments_type code_pointer;
} Mark;

typedef struct Jmp
{
    char*          jmp_mark_name;
    arguments_type code_pointer;
    bool           flag_seen;
} Jmp;
typedef struct Assembler
{
    const char* input_file_path;
    size_t      input_file_size;
    char*       input_data;

    const char* output_file_path;
    size_t      output_file_size;
    uint8_t*    output_data;

    Jmp*        jmp_list;      // NOTE было бы классно конечно использовать список
    size_t      jmp_list_size;

    Mark*       mark_list;
    size_t      mark_list_size;
} Assembler;

static MachineCommands convertCommandToMachineCode(const char* const command);
static Registers convertRegisterToMachineCode(const char* const register_given);
static AssemblerErrorHandler convertFileToMachineCode(Assembler* const assembler_struct);
static AssemblerErrorHandler readDataFromAsmFile(Assembler* const assembler_struct);
static AssemblerErrorHandler callocAssemblerStructArrays(Assembler* const assembler_struct);
static AssemblerErrorHandler assemblerCtor(Assembler* const assembler_struct);
static AssemblerErrorHandler assemblerDtor(Assembler* const assembler_struct);

static const size_t SIZE_OF_BUFFER    = 256;
static const size_t SIZE_OF_MARK_LIST = 32;

static const uint8_t RAM_FLAG      = 0b10000000;
static const uint8_t REGISTER_FLAG = 0b01000000;
static const uint8_t CONST_FLAG    = 0b00100000;

// public --------------------------------------------------------------------------------------------------------------


AssemblerErrorHandler assembleFile(const char* input_file_path, const char* output_file_path)
{
    assert(input_file_path  != NULL);
    assert(output_file_path != NULL);

    Assembler assembler = {};

    assembler.input_file_path  = input_file_path;
    assembler.output_file_path = output_file_path;

    assemblerCtor(&assembler);
    Log(LogLevel_INFO, "Ctor passed");

    convertFileToMachineCode(&assembler);
    Log(LogLevel_INFO, "Converting passed");
    Log(LogLevel_INFO, "pointer on output data: %p\n"
                       "pointer on input data: %p\n"
                       "pointer on jmps: %p\n"
                       "pointer on marks: %p\n",
                       assembler.output_data,
                       assembler.input_data,
                       assembler.jmp_list,
                       assembler.mark_list);

    assemblerDtor(&assembler);
    Log(LogLevel_INFO, "Dtor passed");


    return AssemblerErrorHandler_OK;
}


// static --------------------------------------------------------------------------------------------------------------


static AssemblerErrorHandler assemblerCtor(Assembler* const assembler_struct)
{
    assert(assembler_struct != NULL);

    readDataFromAsmFile(assembler_struct);
    callocAssemblerStructArrays(assembler_struct);

    return AssemblerErrorHandler_OK;
}


static AssemblerErrorHandler callocAssemblerStructArrays(Assembler* const assembler_struct)
{
    assert(assembler_struct != NULL);

    // FIXME добавить динамическое выделение памяти

    size_t len = assembler_struct->input_file_size;
    while(len > 0 && assembler_struct->input_data[len - 1] == '\n')
    {
        len--;
        assembler_struct->input_data[len] = '\0';
    }

    assembler_struct->output_data = (uint8_t*)calloc(assembler_struct->input_file_size, sizeof(uint8_t));
    if (!assembler_struct->output_data)
    {
        return AssemblerErrorHandler_ERROR;
    }
    assembler_struct->output_file_size = 0;

    assembler_struct->jmp_list = (Jmp*)calloc(SIZE_OF_MARK_LIST, sizeof(Jmp));
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

    return AssemblerErrorHandler_OK;
}


static AssemblerErrorHandler readDataFromAsmFile(Assembler* const assembler_struct)
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

    for (size_t current_mark = 0; current_mark < assembler_struct->mark_list_size; current_mark++)
    {
        FREE_NULL(assembler_struct->mark_list[current_mark].mark_name);
    }

    for (size_t current_jmp = 0; current_jmp < assembler_struct->jmp_list_size; current_jmp++)
    {
        FREE_NULL(assembler_struct->jmp_list[current_jmp].jmp_mark_name);
    }

    FREE_NULL(assembler_struct->mark_list);
    FREE_NULL(assembler_struct->jmp_list);

    Log(LogLevel_INFO, "freed all hear");

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

    RETURN_MACHINE_CODE_(command, HLT_COMMAND,  strlen(HLT_COMMAND),  MachineCommands_HLT);
    RETURN_MACHINE_CODE_(command, PUSH_COMMAND, strlen(PUSH_COMMAND), MachineCommands_PUSH);
    RETURN_MACHINE_CODE_(command, POP_COMMAND,  strlen(POP_COMMAND),  MachineCommands_POP)
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

static Registers convertRegisterToMachineCode(const char* const register_given)
{
    assert(register_given != NULL);

#define RETURN_REGISTER_CODE_(register_given, to_compare, number, out) \
    if (!strncmp(register_given, to_compare, number)) return out;

    RETURN_REGISTER_CODE_(register_given, AX_REGISTER, strlen(AX_REGISTER), Registers_AX);
    RETURN_REGISTER_CODE_(register_given, BX_REGISTER, strlen(BX_REGISTER), Registers_BX);
    RETURN_REGISTER_CODE_(register_given, CX_REGISTER, strlen(CX_REGISTER), Registers_CX);
    RETURN_REGISTER_CODE_(register_given, DX_REGISTER, strlen(DX_REGISTER), Registers_DX);
    RETURN_REGISTER_CODE_(register_given, EX_REGISTER, strlen(EX_REGISTER), Registers_EX);
    RETURN_REGISTER_CODE_(register_given, FX_REGISTER, strlen(FX_REGISTER), Registers_FX);

#undef RETURN_REGISTER_CODE_

    return Register_UNKNOWN;
}


static AssemblerErrorHandler convertFileToMachineCode(Assembler* const assembler_struct)
{
    assert(assembler_struct != NULL);

    char* ptr_of_string = strtok(assembler_struct->input_data, "\n");
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

        Log(LogLevel_INFO, "Mark: %s len: %d", mark, mark_size);

        if (last_simbol == ':')
        {
            assembler_struct->mark_list[assembler_struct->mark_list_size] = {
                .mark_name    = (char*)calloc((size_t)mark_size + 1, sizeof(char*)),
                .code_pointer = (arguments_type)assembler_struct->output_file_size,
            };

            strcpy(assembler_struct->mark_list[assembler_struct->mark_list_size].mark_name, mark);

            assembler_struct->mark_list_size++;

            for (size_t current_jmp = 0; current_jmp < assembler_struct->jmp_list_size; current_jmp++)
            {
                if (assembler_struct->jmp_list[current_jmp].flag_seen == false)
                {
                    if (!strcmp(mark, assembler_struct->jmp_list[current_jmp].jmp_mark_name))
                    {
                        memcpy(assembler_struct->output_data + assembler_struct->jmp_list[current_jmp].code_pointer,
                               &assembler_struct->output_file_size,
                               sizeof(arguments_type));

                        assembler_struct->jmp_list[current_jmp].flag_seen = true;
                        break;
                    }
                }
            }

            continue;
        }

        MachineCommands returned_command = convertCommandToMachineCode(command_buffer);

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
                sscanf(string_argument, "%[^:]%n%c", mark, &mark_size, &last_simbol);

                if (last_simbol != ':')
                {
                    return AssemblerErrorHandler_ERROR;
                }

                bool flag_mark_met = false;

                for (size_t current_mark = 0; current_mark < assembler_struct->mark_list_size; current_mark++)
                {
                    if (!strcmp(mark, assembler_struct->mark_list[current_mark].mark_name))
                    {
                        memcpy(assembler_struct->output_data + assembler_struct->output_file_size,
                               &assembler_struct->mark_list[current_mark].code_pointer,
                               sizeof(arguments_type));

                        flag_mark_met = true;
                        break;
                    }
                }

                if (flag_mark_met == false)
                {
                    assembler_struct->jmp_list[assembler_struct->jmp_list_size] = {
                        .jmp_mark_name = (char*)calloc((size_t)mark_size + 1, sizeof(char)),
                        .code_pointer  = (int)assembler_struct->output_file_size,
                        .flag_seen     = false,
                    };

                    strcpy(assembler_struct->jmp_list[assembler_struct->jmp_list_size].jmp_mark_name, mark);

                    assembler_struct->jmp_list_size++;
                }

                assembler_struct->output_file_size += sizeof(arguments_type);
            }

            if (returned_command == MachineCommands_PUSH)
            {
                arguments_type number_argument = 0;
                if (sscanf(string_argument, "%d", &number_argument) == 1)
                {
                    memcpy(assembler_struct->output_data + assembler_struct->output_file_size,
                       &number_argument,
                       sizeof(arguments_type));

                    uint8_t command_with_flag = CONST_FLAG | (uint8_t)returned_command;

                    assembler_struct->output_data[assembler_struct->output_file_size - 1] = command_with_flag;

                    assembler_struct->output_file_size += sizeof(arguments_type);
                }
                else
                {
                    Registers returned_register = convertRegisterToMachineCode(string_argument);
                    if (returned_register == Register_UNKNOWN)
                    {
                        return AssemblerErrorHandler_ERROR;
                    }

                    uint8_t command_with_flag = REGISTER_FLAG | (uint8_t)returned_command;

                    assembler_struct->output_data[assembler_struct->output_file_size - 1] = command_with_flag;
                    assembler_struct->output_data[assembler_struct->output_file_size]     = (uint8_t)returned_register;

                    assembler_struct->output_file_size += sizeof(uint8_t);
                }
            }

            if (returned_command == MachineCommands_POP)
            {
                Registers returned_register = convertRegisterToMachineCode(string_argument);
                if (returned_register == Register_UNKNOWN)
                {
                    return AssemblerErrorHandler_ERROR;
                }

                assembler_struct->output_data[assembler_struct->output_file_size] = (uint8_t)returned_register;

                assembler_struct->output_file_size++;
            }
        }
    }
    while ((ptr_of_string = strtok(NULL, "\n")) != NULL);

    return AssemblerErrorHandler_OK;
}
