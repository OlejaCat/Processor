#include "command_handler.h"

#include <string.h>

MachineCommands convertAsmToMachine(const char* command)
{
#define __RETURN_MACHINE_CODE__(command_given, to_compare, number, out) \
    if (!strncmp(command, to_compare, number)) return out;

    __RETURN_MACHINE_CODE__(command, PUSH_COMMAND, 4, MachineCommands_PUSH);
    __RETURN_MACHINE_CODE__(command, ADD_COMMAND,  3, MachineCommands_ADD);
    __RETURN_MACHINE_CODE__(command, MUL_COMMAND,  3, MachineCommands_MUL);
    __RETURN_MACHINE_CODE__(command, SUB_COMMAND,  3, MachineCommands_SUB);
    __RETURN_MACHINE_CODE__(command, DIV_COMMAND,  3, MachineCommands_DIV);
    __RETURN_MACHINE_CODE__(command, OUT_COMMAND,  3, MachineCommands_OUT);
    __RETURN_MACHINE_CODE__(command, IN_COMMAND,   2, MachineCommands_IN);
    __RETURN_MACHINE_CODE__(command, HLT_COMMAND,  3, MachineCommands_HLT);

#undef __RETURN_MACHINE_CODE__

    return MachineCommands_UNKNOWN;
}

