#ifndef ASSEMBLER_H
#define ASSEMBLER_H

typedef enum AssemblerErrorHandler
{
    AssemblerErrorHandler_OK    = 0,
    AssemblerErrorHandler_ERROR = 1,
} AssemblerErrorHandler;

AssemblerErrorHandler assembleFileToMachineCode(const char* input_file, const char* output_file);

#endif // ASSEMBLER_H
