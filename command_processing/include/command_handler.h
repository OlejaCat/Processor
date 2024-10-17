#ifndef COMMAND_HANDLER_H
#define COMMAND_HANDLER_H

__attribute__((unused)) static const char* PUSH_COMMAND = "PUSH";
__attribute__((unused)) static const char* ADD_COMMAND  = "ADD";
__attribute__((unused)) static const char* MUL_COMMAND  = "MUL";
__attribute__((unused)) static const char* SUB_COMMAND  = "SUB";
__attribute__((unused)) static const char* DIV_COMMAND  = "DIV";
__attribute__((unused)) static const char* OUT_COMMAND  = "OUT";
__attribute__((unused)) static const char* IN_COMMAND   = "IN";
__attribute__((unused)) static const char* HLT_COMMAND  = "HLT";

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
} MachineCommands;

MachineCommands convertAsmToMachine(const char* command);

#endif // COMMAND_HANDLER_H

