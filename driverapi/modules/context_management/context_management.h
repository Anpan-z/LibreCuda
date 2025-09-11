#ifndef LIBRECUDA_CONTEXT_MANAGEMENT_H_
#define LIBRECUDA_CONTEXT_MANAGEMENT_H_

#include "librecuda.h"

#ifdef __cplusplus
extern "C" {
#endif

// Context management functions - these wrap the existing librecuda functions
// This module will eventually contain the extracted context management logic

libreCudaStatus_t contextManagementCreate(LibreCUcontext *pCtx, int flags, LibreCUdevice device);
libreCudaStatus_t contextManagementDestroy(LibreCUcontext ctx);
libreCudaStatus_t contextManagementSetCurrent(LibreCUcontext ctx);
libreCudaStatus_t contextManagementGetCurrent(LibreCUcontext *pCtxOut);

#ifdef __cplusplus
}
#endif

#endif // LIBRECUDA_CONTEXT_MANAGEMENT_H_