#ifndef PROCESSOR_H
#define PROCESSOR_H

typedef enum ProcessorErrorHandler
{
    ProcessorErrorHandler_OK              = 0,
    ProcessorErrorHandler_ERROR           = 1,
    ProcessorErrorHandler_OPEN_FILE_ERROR = 2,
} ProcessorErrorHandler;

ProcessorErrorHandler executeProgram(const char* path_to_program);

#endif // PROCESSOR_H
