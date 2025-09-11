#include "stream_management.h"
#include "librecuda.h"

// These are wrapper functions that call the existing librecuda functions
// Eventually, we'll move the actual implementation here

libreCudaStatus_t streamManagementCreate(LibreCUstream *pStreamOut, uint32_t flags) {
    return libreCuStreamCreate(pStreamOut, flags);
}

libreCudaStatus_t streamManagementDestroy(LibreCUstream stream) {
    return libreCuStreamDestroy(stream);
}

libreCudaStatus_t streamManagementCommence(LibreCUstream stream) {
    return libreCuStreamCommence(stream);
}

libreCudaStatus_t streamManagementAwait(LibreCUstream stream) {
    return libreCuStreamAwait(stream);
}

libreCudaStatus_t streamManagementEventCreate(LibreCUEvent *pEventOut, uint32_t flags) {
    return libreCuEventCreate(pEventOut, flags);
}

libreCudaStatus_t streamManagementEventRecord(LibreCUEvent event, LibreCUstream stream) {
    return libreCuEventRecord(event, stream);
}

libreCudaStatus_t streamManagementEventSynchronize(LibreCUEvent event) {
    return libreCuEventSynchronize(event);
}

libreCudaStatus_t streamManagementEventElapsedTime(float *pMillisecondsOut, LibreCUEvent start, LibreCUEvent end) {
    return libreCuEventElapsedTime(pMillisecondsOut, start, end);
}

libreCudaStatus_t streamManagementEventDestroy(LibreCUEvent event) {
    return libreCuEventDestroy(event);
}