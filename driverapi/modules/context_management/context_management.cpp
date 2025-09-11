#include "context_management.h"
#include "librecuda.h"

// These are wrapper functions that call the existing librecuda functions
// Eventually, we'll move the actual implementation here

libreCudaStatus_t contextManagementCreate(LibreCUcontext *pCtx, int flags, LibreCUdevice device) {
    return libreCuCtxCreate_v2(pCtx, flags, device);
}

libreCudaStatus_t contextManagementDestroy(LibreCUcontext ctx) {
    return libreCuCtxDestroy(ctx);
}

libreCudaStatus_t contextManagementSetCurrent(LibreCUcontext ctx) {
    return libreCuCtxSetCurrent(ctx);
}

libreCudaStatus_t contextManagementGetCurrent(LibreCUcontext *pCtxOut) {
    return libreCuCtxGetCurrent(pCtxOut);
}