#ifndef LIBRECUDA_UVM_IOCTL_H_
#define LIBRECUDA_UVM_IOCTL_H_

#include "librecuda_status.h"

#ifdef __cplusplus
extern "C" {
#endif

// UVM (Unified Virtual Memory) ioctl operations
libreCudaStatus_t uvmInitialize();
libreCudaStatus_t uvmInitializeMM();

#ifdef __cplusplus
}
#endif

#endif // LIBRECUDA_UVM_IOCTL_H_