#ifndef COMMAND_HANDLER_H
#define COMMAND_HANDLER_H

// FIXME attribute лучшн обвернуть в макрос который по разному разворачивыается в зависимости от компилятора
__attribute__((unused)) static const char* HLT_COMMAND  = "HLT";
__attribute__((unused)) static const char* PUSH_COMMAND = "PUSH";
__attribute__((unused)) static const char* POP_COMMAND  = "POP";
__attribute__((unused)) static const char* ADD_COMMAND  = "ADD";
__attribute__((unused)) static const char* MUL_COMMAND  = "MUL";
__attribute__((unused)) static const char* SUB_COMMAND  = "SUB";
__attribute__((unused)) static const char* DIV_COMMAND  = "DIV";
__attribute__((unused)) static const char* OUT_COMMAND  = "OUT";
__attribute__((unused)) static const char* IN_COMMAND   = "IN";
__attribute__((unused)) static const char* JMP_COMMAND  = "JMP";
__attribute__((unused)) static const char* JA_COMMAND   = "JA";
__attribute__((unused)) static const char* JAE_COMMAND  = "JAE";
__attribute__((unused)) static const char* JB_COMMAND   = "JB";
__attribute__((unused)) static const char* JBE_COMMAND  = "JBE";
__attribute__((unused)) static const char* JE_COMMAND   = "JE";
__attribute__((unused)) static const char* JNE_COMMAND  = "JNE";

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
    MachineCommands_OUT     =  7,
    MachineCommands_IN      =  8,
    MachineCommands_JMP     =  9,
    MachineCommands_JA      =  10,
    MachineCommands_JAE     =  11,
    MachineCommands_JB      =  12,
    MachineCommands_JBE     =  13,
    MachineCommands_JE      =  14,
    MachineCommands_JNE     =  16,
} MachineCommands;

__attribute__((unused)) static const char* AX_REGISTER = "AX";
__attribute__((unused)) static const char* BX_REGISTER = "BX";
__attribute__((unused)) static const char* CX_REGISTER = "CX";
__attribute__((unused)) static const char* DX_REGISTER = "DX";
__attribute__((unused)) static const char* EX_REGISTER = "EX";
__attribute__((unused)) static const char* FX_REGISTER = "FX";

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

