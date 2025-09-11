#ifndef LIBRECUDA_MEMORY_MANAGEMENT_H_
#define LIBRECUDA_MEMORY_MANAGEMENT_H_

#include "librecuda.h"

#ifdef __cplusplus
extern "C" {
#endif

// Memory management functions - these wrap the existing librecuda functions
// This module will eventually contain the extracted memory management logic

libreCudaStatus_t memoryManagementAlloc(void **pDevicePointer, size_t bytesize, bool mapToCpu = false);
libreCudaStatus_t memoryManagementFree(void *devicePointer);
libreCudaStatus_t memoryManagementCopy(void *dst, void *src, size_t byteCount, LibreCUstream stream, bool async = false);

#ifdef __cplusplus
}
#endif

#endif // LIBRECUDA_MEMORY_MANAGEMENT_H_