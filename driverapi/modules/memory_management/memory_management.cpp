#include "memory_management.h"
#include "librecuda.h"

// These are wrapper functions that call the existing librecuda functions
// Eventually, we'll move the actual implementation here

libreCudaStatus_t memoryManagementAlloc(void **pDevicePointer, size_t bytesize, bool mapToCpu) {
    return libreCuMemAlloc(pDevicePointer, bytesize, mapToCpu);
}

libreCudaStatus_t memoryManagementFree(void *devicePointer) {
    return libreCuMemFree(devicePointer);
}

libreCudaStatus_t memoryManagementCopy(void *dst, void *src, size_t byteCount, LibreCUstream stream, bool async) {
    return libreCuMemCpy(dst, src, byteCount, stream, async);
}