#include "device_management.h"
#include "librecuda.h"

// These are wrapper functions that call the existing librecuda functions
// Eventually, we'll move the actual implementation here

libreCudaStatus_t deviceManagementInit(int flags) {
    return libreCuInit(flags);
}

libreCudaStatus_t deviceManagementGetCount(int *pDeviceCount) {
    return libreCuDeviceGetCount(pDeviceCount);
}

libreCudaStatus_t deviceManagementGet(LibreCUdevice *device, int deviceOrdinal) {
    return libreCuDeviceGet(device, deviceOrdinal);
}

libreCudaStatus_t deviceManagementGetName(char *pDeviceName, int length, LibreCUdevice device) {
    return libreCuDeviceGetName(pDeviceName, length, device);
}

libreCudaStatus_t deviceManagementGetAttribute(int *pValOut, LibreCuDeviceAttribute attribute, LibreCUdevice device) {
    return libreCuDeviceGetAttribute(pValOut, attribute, device);
}