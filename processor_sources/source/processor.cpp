#include "processor.h"

#include <stdio.h>
#include <string.h>
#include <math.h>
#include <assert.h>

#include "helpful_functions.h"
#include "command_handler.h"
#include "work_with_doubles.h"
#include "stack.h"
#include "dump.h"

#include "logger.h"


// static --------------------------------------------------------------------------------------------------------------


typedef double arguments_type;

static const size_t NUMBER_OF_REGISTERS = 6;
static const size_t SIZE_OF_RAM = 256;
static const uint8_t COMMAND_MASK = 0b0001'1111;
static const uint8_t FLAG_MOVED_MASK = 0b1110'0000;

typedef struct SPU
{
    uint8_t*        code;
    size_t          ip;
    size_t          size_of_code;

    Stack*          program_stack;
    Stack*          function_stack;

    arguments_type* registers;
    arguments_type* ram;

    bool            end_flag;
} SPU;

static ProcessorErrorHandler readProgramCode(const char* path_to_program, SPU* const spu);
static ProcessorErrorHandler spuInit(const char* path_to_program, SPU* const spu);
static ProcessorErrorHandler spuDtor(SPU* const spu);

static void processMachineCode(SPU* const spu);

static void pushArgument(SPU* const spu);
static void popRegister(SPU* const spu);
static void addCommand(SPU* const spu);
static void mulCommand(SPU* const spu);
static void subCommand(SPU* const spu);
static void divCommand(SPU* const spu);
static void sqrtCommand(SPU* const spu);
static void outCommand(SPU* const spu);
static void inCommand(SPU* const spu);
static void hltCommand(SPU* const spu);
static void jmpCommand(SPU* const spu);
static void jaCommand(SPU* const spu);
static void jaeCommand(SPU* const spu);
static void jbCommand(SPU* const spu);
static void jbeCommand(SPU* const spu);
static void jeCommand(SPU* const spu);
static void jneCommand(SPU* const spu);
static void retCommand(SPU* const spu);
static void callCommand(SPU* const spu);


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
    assert(spu != NULL);

    while (spu->end_flag)
    {
        switch(spu->code[spu->ip] & COMMAND_MASK)
        {
            case MachineCommands_PUSH: pushArgument(spu); break;

            case MachineCommands_POP:  popRegister(spu);  break;

            case MachineCommands_ADD:  addCommand(spu);   break;

            case MachineCommands_MUL:  mulCommand(spu);   break;

            case MachineCommands_DIV:  divCommand(spu);   break;

            case MachineCommands_SUB:  subCommand(spu);   break;

            case MachineCommands_SQRT: sqrtCommand(spu);  break;

            case MachineCommands_OUT:  outCommand(spu);   break;

            case MachineCommands_IN:   inCommand(spu);    break;

            case MachineCommands_JMP:  jmpCommand(spu);   break;

            case MachineCommands_JA:   jaCommand(spu);    break;

            case MachineCommands_JAE:  jaeCommand(spu);   break;

            case MachineCommands_JB:   jbCommand(spu);    break;

            case MachineCommands_JBE:  jbeCommand(spu);   break;

            case MachineCommands_JE:   jeCommand(spu);    break;

            case MachineCommands_JNE:  jneCommand(spu);   break;

            case MachineCommands_RET:  retCommand(spu);   break;

            case MachineCommands_CALL: callCommand(spu);  break;

            case MachineCommands_HLT:  hltCommand(spu);   break;

            default:                   assert(0 && "Unknown command");
        }
        writeStackDumpLog(spu->program_stack);
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

    spu->ip             = 0;
    spu->program_stack  = stackCtor();
    spu->function_stack = stackCtor();
    spu->end_flag       = true;

    spu->registers = (arguments_type*)calloc(NUMBER_OF_REGISTERS + 1,
                                             sizeof(arguments_type));
    if (!spu->registers)
    {
        return ProcessorErrorHandler_ERROR;
    }

    spu->ram = (arguments_type*)calloc(SIZE_OF_RAM,
                                       sizeof(arguments_type));
    if (!spu->ram)
    {
        return ProcessorErrorHandler_ERROR;
    }

    return ProcessorErrorHandler_OK;
}


static ProcessorErrorHandler spuDtor(SPU* spu)
{
    assert(spu != NULL);

    free(spu->code);
    free(spu->registers);
    free(spu->ram);

    stackDtor(spu->program_stack);
    stackDtor(spu->function_stack);

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

    spu->size_of_code = getFileSize(program_file);

    spu->code = (uint8_t*)calloc(spu->size_of_code, sizeof(uint8_t));
    if (!spu->code)
    {
        FCLOSE_NULL(program_file);
        return ProcessorErrorHandler_ERROR;
    }

    size_t return_code = fread(spu->code,
                               sizeof(spu->code[0]),
                               spu->size_of_code,
                               program_file);
    if (return_code != spu->size_of_code)
    {
        FREE_NULL(spu->code);
        FCLOSE_NULL(program_file);
        return ProcessorErrorHandler_ERROR;
    }

    return ProcessorErrorHandler_OK;
}


static void pushArgument(SPU* const spu)
{
    assert(spu != NULL);

    uint8_t flags = spu->code[spu->ip] & FLAG_MOVED_MASK;

    arguments_type argument = 0;

    if (flags & REGISTER_FLAG)
    {
        spu->ip++;
        argument = spu->registers[spu->code[spu->ip]];
    }

    if (flags & CONST_FLAG)
    {
        arguments_type const_argument = 0;

        memcpy(&const_argument, spu->code + spu->ip + 1, sizeof(arguments_type));
        spu->ip += sizeof(arguments_type);

        argument += const_argument;
    }

    if (flags & RAM_FLAG)
    {
        argument = spu->ram[(size_t)argument];
    }

    stackPush(spu->program_stack, argument);
}


static void popRegister(SPU* const spu)
{
    assert(spu != NULL);

    uint8_t flags = spu->code[spu->ip] & FLAG_MOVED_MASK;

    if (flags & RAM_FLAG)
    {
        size_t index = 0;

        if (flags & REGISTER_FLAG)
        {
            spu->ip++;
            index = (size_t)(*(spu->registers + spu->code[spu->ip]));
        }

        if (flags & CONST_FLAG)
        {
            size_t const_argument = 0;

            memcpy(&const_argument, spu->code + spu->ip + 1, sizeof(arguments_type));
            spu->ip += sizeof(arguments_type);

            index += const_argument;
        }

        stackPop(spu->program_stack, spu->ram + index);
    }
    else
    {
        spu->ip++;
        stackPop(spu->program_stack, spu->registers + spu->code[spu->ip]);
    }
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


static void subCommand(SPU* const spu)
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


static void sqrtCommand(SPU* const spu)
{
    assert(spu != NULL);

    arguments_type element = 0;

    stackPop(spu->program_stack, &element);

    arguments_type result = 0;
    result = sqrt(element);

    stackPush(spu->program_stack, result);
}


static void outCommand(SPU* const spu)
{
    assert(spu != NULL);

    arguments_type element = 0;
    stackPop(spu->program_stack, &element);

    printf("Program out: %lg\n", element);
}


static void inCommand(SPU* const spu)
{
    assert(spu != NULL);

    arguments_type element = 0;

    printf("Enter argument: ");
    scanf("%lg", &element);

    stackPush(spu->program_stack, element);
}


static void jmpCommand(SPU* const spu)
{
    assert(spu != NULL);

    size_t argument = 0;

    memcpy(&argument, spu->code + spu->ip + 1, sizeof(arguments_type));
    spu->ip = argument - 1;
}


static void jaCommand(SPU* const spu)
{
    assert(spu != NULL);

    arguments_type first_element  = 0;
    arguments_type second_element = 0;

    stackPop(spu->program_stack, &first_element);
    stackPop(spu->program_stack, &second_element);

    if (first_element > second_element)
    {
        size_t argument = 0;

        memcpy(&argument, spu->code + spu->ip + 1, sizeof(arguments_type));
        spu->ip = argument - 1;
    }
    else
    {
        spu->ip += sizeof(arguments_type);
    }
}


static void     jaeCommand(SPU* const spu)
{
    assert(spu != NULL);

    arguments_type first_element  = 0;
    arguments_type second_element = 0;

    stackPop(spu->program_stack, &first_element);
    stackPop(spu->program_stack, &second_element);

    if (first_element >= second_element)
    {
        size_t argument = 0;

        memcpy(&argument, spu->code + spu->ip + 1, sizeof(arguments_type));
        spu->ip = argument - 1;
    }
    else
    {
        spu->ip += sizeof(arguments_type);
    }
}


static void jbCommand(SPU* const spu)
{
    assert(spu != NULL);

    arguments_type first_element  = 0;
    arguments_type second_element = 0;

    stackPop(spu->program_stack, &first_element);
    stackPop(spu->program_stack, &second_element);

    if (first_element < second_element)
    {
        size_t argument = 0;

        memcpy(&argument, spu->code + spu->ip + 1, sizeof(arguments_type));
        spu->ip = argument - 1;
    }
    else
    {
        spu->ip += sizeof(arguments_type);
    }
}


static void jbeCommand(SPU* const spu)
{
    assert(spu != NULL);

    arguments_type first_element  = 0;
    arguments_type second_element = 0;

    stackPop(spu->program_stack, &first_element);
    stackPop(spu->program_stack, &second_element);

    if (first_element <= second_element)
    {
        size_t argument = 0;

        memcpy(&argument, spu->code + spu->ip + 1, sizeof(arguments_type));
        spu->ip = argument - 1;
    }
    else
    {
        spu->ip += sizeof(arguments_type);
    }
}


static void jeCommand(SPU* const spu)
{
    assert(spu != NULL);

    arguments_type first_element  = 0;
    arguments_type second_element = 0;

    stackPop(spu->program_stack, &first_element);
    stackPop(spu->program_stack, &second_element);

    if (equatTwoDoubles(first_element, second_element))
    {
        size_t argument = 0;

        memcpy(&argument, spu->code + spu->ip + 1, sizeof(arguments_type));
        spu->ip = argument - 1;
    }
    else
    {
        spu->ip += sizeof(arguments_type);
    }
}


static void jneCommand(SPU* const spu)
{
    assert(spu != NULL);

    arguments_type first_element  = 0;
    arguments_type second_element = 0;

    stackPop(spu->program_stack, &first_element);
    stackPop(spu->program_stack, &second_element);

    if (!equatTwoDoubles(first_element, second_element))
    {
        size_t argument = 0;

        memcpy(&argument, spu->code + spu->ip + 1, sizeof(arguments_type));
        spu->ip = argument - 1;
    }
    else
    {
        spu->ip += sizeof(arguments_type);
    }
}


static void retCommand(SPU* const spu)
{
    assert(spu != NULL);

    arguments_type pointer_double = 0;
    stackPop(spu->function_stack, &pointer_double);

    size_t pointer = 0;
    memcpy(&pointer, &pointer_double, sizeof(arguments_type));

    spu->ip = pointer - 1;
}


static void callCommand(SPU* const spu)
{
    assert(spu != NULL);

    size_t pointer = spu->ip + sizeof(arguments_type);
    arguments_type pointer_double = 0;

    memcpy(&pointer_double, &pointer, sizeof(arguments_type));

    stackPush(spu->function_stack, pointer_double);

    size_t argument = 0;
    memcpy(&argument, spu->code + spu->ip + 1, sizeof(arguments_type));

    spu->ip = argument - 1;
}


static void hltCommand(SPU* const spu)
{
    assert(spu != NULL);

    spu->end_flag = false;

    writeStackDumpLog(spu->program_stack);

    printf("Program end\n");
}
