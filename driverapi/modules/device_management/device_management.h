#ifndef LIBRECUDA_DEVICE_MANAGEMENT_H_
#define LIBRECUDA_DEVICE_MANAGEMENT_H_

#include "librecuda.h"

#ifdef __cplusplus
extern "C" {
#endif

// Device management functions - these wrap the existing librecuda functions
// This module will eventually contain the extracted device management logic

// Initialize device subsystem
libreCudaStatus_t deviceManagementInit(int flags);

// Device enumeration and information
libreCudaStatus_t deviceManagementGetCount(int *pDeviceCount);
libreCudaStatus_t deviceManagementGet(LibreCUdevice *device, int deviceOrdinal);
libreCudaStatus_t deviceManagementGetName(char *pDeviceName, int length, LibreCUdevice device);
libreCudaStatus_t deviceManagementGetAttribute(int *pValOut, LibreCuDeviceAttribute attribute, LibreCUdevice device);

#ifdef __cplusplus
}
#endif

#endif // LIBRECUDA_DEVICE_MANAGEMENT_H_