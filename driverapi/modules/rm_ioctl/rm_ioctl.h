#ifndef LIBRECUDA_RM_IOCTL_H_
#define LIBRECUDA_RM_IOCTL_H_

#include "librecuda_status.h"

#ifdef __cplusplus
extern "C" {
#endif

// Forward declarations from nvidia headers
typedef unsigned int NvU32;
typedef int NvV32;
typedef NvU32 NvHandle;

// RM (Resource Manager) ioctl wrapper
libreCudaStatus_t rmCtrlCall(int fd, NvV32 cmd, NvHandle client, NvHandle object, void *params, NvU32 paramSize);

// Initialize RM subsystem
libreCudaStatus_t rmInit();

// Get RM file descriptor  
int rmGetFd();

// Get root client handle
NvHandle rmGetRootClient();

#ifdef __cplusplus
}
#endif

#endif // LIBRECUDA_RM_IOCTL_H_