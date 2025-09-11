#ifndef LIBRECUDA_STREAM_MANAGEMENT_H_
#define LIBRECUDA_STREAM_MANAGEMENT_H_

#include "librecuda.h"

#ifdef __cplusplus
extern "C" {
#endif

// Stream management functions - these wrap the existing librecuda functions
// This module will eventually contain the extracted stream management logic

libreCudaStatus_t streamManagementCreate(LibreCUstream *pStreamOut, uint32_t flags);
libreCudaStatus_t streamManagementDestroy(LibreCUstream stream);
libreCudaStatus_t streamManagementCommence(LibreCUstream stream);
libreCudaStatus_t streamManagementAwait(LibreCUstream stream);

// Event management
libreCudaStatus_t streamManagementEventCreate(LibreCUEvent *pEventOut, uint32_t flags);
libreCudaStatus_t streamManagementEventRecord(LibreCUEvent event, LibreCUstream stream);
libreCudaStatus_t streamManagementEventSynchronize(LibreCUEvent event);
libreCudaStatus_t streamManagementEventElapsedTime(float *pMillisecondsOut, LibreCUEvent start, LibreCUEvent end);
libreCudaStatus_t streamManagementEventDestroy(LibreCUEvent event);

#ifdef __cplusplus
}
#endif

#endif // LIBRECUDA_STREAM_MANAGEMENT_H_