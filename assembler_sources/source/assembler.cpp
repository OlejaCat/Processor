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
static AssemblerErrorHandler convertFileToMachineCode(Assembler* const assembler);
static void saveMarkToMarkList(Assembler* const assembler, char* mark, size_t mark_size);
static void saveMarkToJmpList(Assembler* const assembler, char* mark, size_t mark_size);
static AssemblerErrorHandler parseArgument(Assembler* const assembler, char* argument);
static bool checkIfMark(char* command_buffer);
static bool checkRegisterValidity(char* buffer);
static AssemblerErrorHandler processJmpCommand(Assembler* const assembler,
                                               char*            argument);

static AssemblerErrorHandler readDataFromAsmFile(Assembler* const assembler);
static AssemblerErrorHandler callocAssemblerStructArrays(Assembler* const assembler);
static AssemblerErrorHandler assemblerCtor(Assembler* const assembler);
static AssemblerErrorHandler assemblerDtor(Assembler* const assembler);

static const size_t SIZE_OF_BUFFER    = 128;
static const size_t SIZE_OF_MARK_LIST = 32;

static const uint8_t RAM_FLAG      = 0b10000000;
static const uint8_t REGISTER_FLAG = 0b01000000;
static const uint8_t CONST_FLAG    = 0b00100000;

#define RETURN_CHECK_MARK \



// public --------------------------------------------------------------------------------------------------------------


AssemblerErrorHandler assembleFile(const char* input_file_path,
                                   const char* output_file_path)
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


static AssemblerErrorHandler assemblerCtor(Assembler* const assembler)
{
    assert(assembler != NULL);

    assembler->input_data  = NULL;
    assembler->mark_list   = NULL;
    assembler->jmp_list    = NULL;
    assembler->output_data = NULL;

    readDataFromAsmFile(assembler);
    callocAssemblerStructArrays(assembler);

    return AssemblerErrorHandler_OK;
}


static AssemblerErrorHandler callocAssemblerStructArrays(Assembler* const assembler)
{
    assert(assembler != NULL);

    size_t len = assembler->input_file_size;
    while(len > 0 && assembler->input_data[len - 1] == '\n')
    {
        len--;
        assembler->input_data[len] = '\0';
    }

    assembler->output_data = (uint8_t*)calloc(assembler->input_file_size, sizeof(uint8_t));
    if (!assembler->output_data)
    {
        return AssemblerErrorHandler_ERROR;
    }
    assembler->output_file_size = 0;

    assembler->jmp_list = (Jmp*)calloc(SIZE_OF_MARK_LIST, sizeof(Jmp));
    if(!assembler->jmp_list)
    {
        return AssemblerErrorHandler_ERROR;
    }
    assembler->jmp_list_size = 0;

    assembler->mark_list = (Mark*)calloc(SIZE_OF_MARK_LIST, sizeof(Mark));
    if (!assembler->mark_list)
    {
        return AssemblerErrorHandler_ERROR;
    }
    assembler->mark_list_size = 0;

    return AssemblerErrorHandler_OK;
}


static AssemblerErrorHandler readDataFromAsmFile(Assembler* const assembler)
{
    assert(assembler != NULL);

    FILE* input_file = fopen(assembler->input_file_path, "r");
    if (!input_file)
    {
        return AssemblerErrorHandler_ERROR;
    }

    assembler->input_file_size = getFileSize(input_file);

    assembler->input_data = (char*)calloc(assembler->input_file_size, sizeof(char));
    if (!assembler->input_data)
    {
        FCLOSE_NULL(input_file);
        return AssemblerErrorHandler_ERROR;
    }

    size_t return_code = fread(assembler->input_data,
                               sizeof(assembler->input_data[0]),
                               assembler->input_file_size,
                               input_file);
    if (return_code != assembler->input_file_size)
    {
        memset(assembler, 0, sizeof(Assembler));
        return AssemblerErrorHandler_ERROR;
    }

    FCLOSE_NULL(input_file);

    return AssemblerErrorHandler_OK;
}


static AssemblerErrorHandler assemblerDtor(Assembler* const assembler)
{
    assert(assembler != NULL);

    FREE_NULL(assembler->input_data);

    for (size_t current_mark = 0; current_mark < assembler->mark_list_size; current_mark++)
    {
        FREE_NULL(assembler->mark_list[current_mark].mark_name);
    }

    for (size_t current_jmp = 0; current_jmp < assembler->jmp_list_size; current_jmp++)
    {
        FREE_NULL(assembler->jmp_list[current_jmp].jmp_mark_name);
    }

    FREE_NULL(assembler->mark_list);
    FREE_NULL(assembler->jmp_list);

    FILE* output_file = fopen(assembler->output_file_path, "wb");
    if (!output_file)
    {
        FREE_NULL(assembler->output_data);
        return AssemblerErrorHandler_ERROR;
    }

    size_t size_writed = fwrite(assembler->output_data,
                                sizeof(assembler->output_data[0]),
                                assembler->output_file_size,
                                output_file);
    if (size_writed != assembler->output_file_size)
    {
        FREE_NULL(assembler->output_data);
        FCLOSE_NULL(output_file);
        return AssemblerErrorHandler_ERROR;
    }

    FREE_NULL(assembler->output_data);
    FCLOSE_NULL(output_file);

    return AssemblerErrorHandler_OK;
}


static MachineCommands convertCommandToMachineCode(const char* const command)
{
    assert(command != NULL);

#define RETURN_MACHINE_CODE_(command_given) \
    if (!strncmp(command, command_given##_COMMAND, strlen(command_given##_COMMAND))) \
        return MachineCommands_##command_given;

    RETURN_MACHINE_CODE_(HLT);
    RETURN_MACHINE_CODE_(PUSH);
    RETURN_MACHINE_CODE_(POP)
    RETURN_MACHINE_CODE_(ADD);
    RETURN_MACHINE_CODE_(MUL);
    RETURN_MACHINE_CODE_(SUB);
    RETURN_MACHINE_CODE_(DIV);
    RETURN_MACHINE_CODE_(OUT);
    RETURN_MACHINE_CODE_(IN);
    RETURN_MACHINE_CODE_(JMP);
    RETURN_MACHINE_CODE_(JA);
    RETURN_MACHINE_CODE_(JAE);
    RETURN_MACHINE_CODE_(JB);
    RETURN_MACHINE_CODE_(JBE);
    RETURN_MACHINE_CODE_(JE);
    RETURN_MACHINE_CODE_(JNE);

#undef  RETURN_MACHINE_CODE_

    return MachineCommands_UNKNOWN;
}

static Registers convertRegisterToMachineCode(const char* const register_given)
{
    assert(register_given != NULL);

#define RETURN_REGISTER_CODE_(reg) \
    if (!strncmp(register_given, reg##_REGISTER, strlen(reg##_REGISTER))) \
        return Registers_##reg;

    RETURN_REGISTER_CODE_(AX);
    RETURN_REGISTER_CODE_(BX);
    RETURN_REGISTER_CODE_(CX);
    RETURN_REGISTER_CODE_(DX);
    RETURN_REGISTER_CODE_(EX);
    RETURN_REGISTER_CODE_(FX);

#undef RETURN_REGISTER_CODE_

    return Register_UNKNOWN;
}


static AssemblerErrorHandler convertFileToMachineCode(Assembler* const assembler)
{
    assert(assembler != NULL);

    char command_buffer[SIZE_OF_BUFFER]  = {};
    char string_argument[SIZE_OF_BUFFER] = {};
    char after_line[SIZE_OF_BUFFER]      = {};

#define COMPARE_RETURNED_COMMAND_(cmd) returned_command == MachineCommands_##cmd

    char* ptr_of_string = strtok(assembler->input_data, "\n");
    do
    {
        sscanf(ptr_of_string, "%s %s%[^;]", command_buffer, string_argument, after_line);

        // NOTE добавить обработку комментов

        if (command_buffer[0] == '\0')
        {
            continue;
        }

        MachineCommands returned_command = convertCommandToMachineCode(command_buffer);

        Log(LogLevel_INFO, "processed command XDDDDD: %d", returned_command);

        assembler->output_data[assembler->output_file_size] = (uint8_t)returned_command;

        if (checkIfMark(command_buffer))
        {
            char mark[SIZE_OF_BUFFER] = {};
            int mark_size             = 0;

            sscanf(command_buffer, "%[^:]%n", mark, &mark_size);

            saveMarkToMarkList(assembler, mark, (size_t)mark_size);

            Log(LogLevel_INFO, "We processed mark: %s", command_buffer);
        }
        else if (COMPARE_RETURNED_COMMAND_(PUSH)
              || COMPARE_RETURNED_COMMAND_(POP))
        {
            parseArgument(assembler, string_argument);
        }
        else if (COMPARE_RETURNED_COMMAND_(ADD)
              || COMPARE_RETURNED_COMMAND_(MUL)
              || COMPARE_RETURNED_COMMAND_(SUB)
              || COMPARE_RETURNED_COMMAND_(DIV)
              || COMPARE_RETURNED_COMMAND_(OUT)
              || COMPARE_RETURNED_COMMAND_(IN)
              || COMPARE_RETURNED_COMMAND_(HLT))
        {
            assembler->output_file_size++;
        }
        else if (COMPARE_RETURNED_COMMAND_(JMP)
              || COMPARE_RETURNED_COMMAND_(JA)
              || COMPARE_RETURNED_COMMAND_(JAE)
              || COMPARE_RETURNED_COMMAND_(JB)
              || COMPARE_RETURNED_COMMAND_(JBE)
              || COMPARE_RETURNED_COMMAND_(JE)
              || COMPARE_RETURNED_COMMAND_(JNE))
        {
            processJmpCommand(assembler, string_argument);
        }
        else
        {
            Log(LogLevel_INFO, "HHOOOOOOOOW BLYAT");
            return AssemblerErrorHandler_ERROR;
        }
    }
    while ((ptr_of_string = strtok(NULL, "\n")) != NULL);

#undef COMPARE_RETURNED_COMMAND_

    return AssemblerErrorHandler_OK;
}


static AssemblerErrorHandler processJmpCommand(Assembler* const assembler,
                                               char*            argument)
{
    Log(LogLevel_INFO, "%s", argument);

    if (!checkIfMark(argument))
    {
        Log(LogLevel_INFO, "DFJSKL HJKLSKLFDKSLSDKL KLS");
        return AssemblerErrorHandler_ERROR;
    }

    char mark[SIZE_OF_BUFFER] = {};
    int  mark_size            = 0;

    sscanf(argument, "%[^:]%n", mark, &mark_size);

    assembler->output_file_size++;

    saveMarkToJmpList(assembler, mark, (size_t)mark_size);

    assembler->output_file_size += sizeof(arguments_type);

    return AssemblerErrorHandler_OK;
}


static AssemblerErrorHandler parseArgument(Assembler* const assembler, char* argument)
{
    assert(assembler != NULL);
    assert(argument  != NULL);

    arguments_type number = 0;


    char first_buffer[SIZE_OF_BUFFER]  = {};
    char second_buffer[SIZE_OF_BUFFER] = {};

    int return_number = 0;

    uint8_t machine_code = assembler->output_data[assembler->output_file_size];

    return_number = sscanf(argument, "[%[^]+]+ %[^]]", first_buffer, second_buffer);
    if (return_number > 0)
    {
        Log(LogLevel_INFO, "JZH x2");

        machine_code |= RAM_FLAG;
    }
    else
    {
        Log(LogLevel_INFO, "JZH");
        return_number = sscanf(argument, "%[^+]+%s", first_buffer, second_buffer);
    }

    Log(LogLevel_INFO, "WOWWWW: 1:%s, 2:%s, n:%d", first_buffer, second_buffer, return_number);

    if (return_number == 2)
    {
        return_number = sscanf(first_buffer, "%d", &number);
        if (return_number == 1)
        {
            sscanf(second_buffer, "%s", first_buffer);

            if (!checkRegisterValidity(second_buffer))
            {
                return AssemblerErrorHandler_ERROR;
            }
        }
        else
        {
            return_number = sscanf(second_buffer, "%d", &number);
            if (return_number == 0)
            {
                return AssemblerErrorHandler_ERROR;
            }

            if (!checkRegisterValidity(first_buffer))
            {
                return AssemblerErrorHandler_ERROR;
            }

            Log(LogLevel_INFO, "WOw x2");
        }

        machine_code |= CONST_FLAG;
        machine_code |= REGISTER_FLAG;
    }
    else if (return_number == 1)
    {
        return_number = sscanf(first_buffer, "%d", &number);
        if (return_number == 1)
        {
            machine_code |= CONST_FLAG;
        }
        else
        {
            if (!checkRegisterValidity(first_buffer))
            {
                return AssemblerErrorHandler_ERROR;
            }

            machine_code |= REGISTER_FLAG;
        }
    }
    else
    {
        return AssemblerErrorHandler_ERROR;
    }

    assembler->output_data[assembler->output_file_size] = machine_code;
    assembler->output_file_size++;

    if ((machine_code & REGISTER_FLAG) == REGISTER_FLAG)
    {
        Registers returned_reg = convertRegisterToMachineCode(first_buffer);
        assembler->output_data[assembler->output_file_size] = returned_reg;
        assembler->output_file_size++;
    }

    if ((machine_code & CONST_FLAG) == CONST_FLAG)
    {
        memcpy(assembler->output_data + assembler->output_file_size,
               &number,
               sizeof(arguments_type));

        assembler->output_file_size += sizeof(arguments_type);
    }

    return AssemblerErrorHandler_OK;
}


static bool checkRegisterValidity(char* buffer)
{
    assert(buffer != NULL);

#define COMPARE_REGISTER_(reg) (strcmp(buffer, reg##_REGISTER) == 0)

    return COMPARE_REGISTER_(AX)
        || COMPARE_REGISTER_(BX)
        || COMPARE_REGISTER_(CX)
        || COMPARE_REGISTER_(DX)
        || COMPARE_REGISTER_(EX)
        || COMPARE_REGISTER_(FX);

#undef COMPARE_
}


static void saveMarkToJmpList(Assembler* const assembler, char* mark, size_t mark_size)
{
    assert(assembler != NULL);
    assert(mark      != NULL);

    bool flag_mark_met = false;

    for (size_t current_mark = 0; current_mark < assembler->mark_list_size; current_mark++)
    {
        if (!strcmp(mark, assembler->mark_list[current_mark].mark_name))
        {
            memcpy(assembler->output_data + assembler->output_file_size,
                   &assembler->mark_list[current_mark].code_pointer,
                   sizeof(arguments_type));

            flag_mark_met = true;
            break;
        }
    }

    if (flag_mark_met == false)
    {
        assembler->jmp_list[assembler->jmp_list_size] = {
            .jmp_mark_name = (char*)calloc(mark_size + 1, sizeof(char)),
            .code_pointer  = (int)assembler->output_file_size,
            .flag_seen     = false,
        };

        strcpy(assembler->jmp_list[assembler->jmp_list_size].jmp_mark_name, mark);

        assembler->jmp_list_size++;
    }
}


static void saveMarkToMarkList(Assembler* const assembler, char* mark, size_t mark_size)
{
    assert(assembler != NULL);
    assert(mark      != NULL);

    assembler->mark_list[assembler->mark_list_size] = {
        .mark_name    = (char*)calloc(mark_size + 1, sizeof(char*)),
        .code_pointer = (arguments_type)assembler->output_file_size,
    };

    strcpy(assembler->mark_list[assembler->mark_list_size].mark_name, mark);

    assembler->mark_list_size++;

    for (size_t current_jmp = 0; current_jmp < assembler->jmp_list_size; current_jmp++)
    {
        if (assembler->jmp_list[current_jmp].flag_seen == false)
        {
            if (!strcmp(mark, assembler->jmp_list[current_jmp].jmp_mark_name))
            {
                memcpy(assembler->output_data + assembler->jmp_list[current_jmp].code_pointer,
                       &assembler->output_file_size,
                       sizeof(arguments_type));

                assembler->jmp_list[current_jmp].flag_seen = true;
                break;
            }
        }
    }
}


static bool checkIfMark(char* buffer)
{
    return strchr(buffer, ':') != NULL ? true : false;
}
