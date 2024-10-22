#ifndef COMMAND_HANDLER_H
#define COMMAND_HANDLER_H

// FIXME attribute лучшн обвернуть в макрос который по разному разворачивыается в зависимости от компилятора
__attribute__((unused)) static const char* PUSH_COMMAND = "PUSH";
__attribute__((unused)) static const char* ADD_COMMAND  = "ADD";
__attribute__((unused)) static const char* MUL_COMMAND  = "MUL";
__attribute__((unused)) static const char* SUB_COMMAND  = "SUB";
__attribute__((unused)) static const char* DIV_COMMAND  = "DIV";
__attribute__((unused)) static const char* OUT_COMMAND  = "OUT";
__attribute__((unused)) static const char* IN_COMMAND   = "IN";
__attribute__((unused)) static const char* HLT_COMMAND  = "HLT";
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
    MachineCommands_ADD     =  2,
    MachineCommands_MUL     =  3,
    MachineCommands_SUB     =  4,
    MachineCommands_DIV     =  5,
    MachineCommands_OUT     =  6,
    MachineCommands_IN      =  7,
    MachineCommands_POP     =  8,
    MachineCommands_JMP     =  9,
    MachineCommands_JA      =  10,
    MachineCommands_JAE     =  11,
    MachineCommands_JB      =  12,
    MachineCommands_JBE     =  13,
    MachineCommands_JE      =  14,
    MachineCommands_JNE     =  16,
} MachineCommands;

#endif // COMMAND_HANDLER_H

