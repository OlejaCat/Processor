#ifndef PROCESSOR_H
#define PROCESSOR_H

typedef enum ProcessorErrorHandler
{
    ProcessorErrorHandler_OK              = 0,
    ProcessorErrorHandler_ERROR           = 1,
    ProcessorErrorHandler_OPEN_FILE_ERROR = 2,
} ProcessorErrorHandler;

#endif // PROCESSOR_H
