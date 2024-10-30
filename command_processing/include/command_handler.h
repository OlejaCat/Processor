#ifndef COMMAND_HANDLER_H
#define COMMAND_HANDLER_H

#include <stdint.h>

static const uint8_t RAM_FLAG      = 0b1000'0000;
static const uint8_t REGISTER_FLAG = 0b0100'0000;
static const uint8_t CONST_FLAG    = 0b0010'0000;

// FIXME attribute лучшн обвернуть в макрос который по разному разворачивыается в зависимости от компилятора
__attribute__((unused)) static const char* HLT_COMMAND        = "hlt";
__attribute__((unused)) static const char* PUSH_COMMAND       = "push";
__attribute__((unused)) static const char* POP_COMMAND        = "pop";
__attribute__((unused)) static const char* ADD_COMMAND        = "add";
__attribute__((unused)) static const char* MUL_COMMAND        = "mul";
__attribute__((unused)) static const char* SUB_COMMAND        = "sub";
__attribute__((unused)) static const char* DIV_COMMAND        = "div";
__attribute__((unused)) static const char* SQRT_COMMAND       = "sqrt";
__attribute__((unused)) static const char* OUT_COMMAND        = "out";
__attribute__((unused)) static const char* IN_COMMAND         = "in";
__attribute__((unused)) static const char* JMP_COMMAND        = "jmp";
__attribute__((unused)) static const char* JA_COMMAND         = "ja";
__attribute__((unused)) static const char* JAE_COMMAND        = "jae";
__attribute__((unused)) static const char* JB_COMMAND         = "jb";
__attribute__((unused)) static const char* JBE_COMMAND        = "jbe";
__attribute__((unused)) static const char* JE_COMMAND         = "je";
__attribute__((unused)) static const char* JNE_COMMAND        = "jne";
__attribute__((unused)) static const char* CALL_COMMAND       = "call";
__attribute__((unused)) static const char* RET_COMMAND        = "ret";
__attribute__((unused)) static const char* FUNC_START_COMMAND = "proc";
__attribute__((unused)) static const char* FUNC_END_COMMAND   = "endp";

typedef enum MachineCommands
{
    MachineCommands_UNKNOWN = -1,
    MachineCommands_HLT     =  0,
    MachineCommands_PUSH    =  1,
    MachineCommands_POP     =  2,
    MachineCommands_ADD     =  3,
    MachineCommands_MUL     =  4,
    MachineCommands_SUB     =  5,
    MachineCommands_DIV     =  6,
    MachineCommands_SQRT    =  7,
    MachineCommands_OUT     =  8,
    MachineCommands_IN      =  9,
    MachineCommands_JMP     =  10,
    MachineCommands_JA      =  11,
    MachineCommands_JAE     =  12,
    MachineCommands_JB      =  13,
    MachineCommands_JBE     =  14,
    MachineCommands_JE      =  15,
    MachineCommands_JNE     =  16,
    MachineCommands_CALL    =  17,
    MachineCommands_RET     =  18,
} MachineCommands;

__attribute__((unused)) static const char* AX_REGISTER = "ax";
__attribute__((unused)) static const char* BX_REGISTER = "bx";
__attribute__((unused)) static const char* CX_REGISTER = "cx";
__attribute__((unused)) static const char* DX_REGISTER = "dx";
__attribute__((unused)) static const char* EX_REGISTER = "ex";
__attribute__((unused)) static const char* FX_REGISTER = "fx";

typedef enum Registers
{
    Register_UNKNOWN = 0,
    Registers_AX     = 1,
    Registers_BX     = 2,
    Registers_CX     = 3,
    Registers_DX     = 4,
    Registers_EX     = 5,
    Registers_FX     = 6,
} Registers;

#endif // COMMAND_HANDLER_H

