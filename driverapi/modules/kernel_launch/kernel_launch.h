#ifndef LIBRECUDA_KERNEL_LAUNCH_H_
#define LIBRECUDA_KERNEL_LAUNCH_H_

#include "librecuda.h"

#ifdef __cplusplus
extern "C" {
#endif

// Kernel launch functions - these wrap the existing librecuda functions
// This module will eventually contain the extracted kernel launch logic

// Module management
libreCudaStatus_t kernelLaunchModuleLoadData(LibreCUmodule *pModuleOut, const void *image, size_t imageSize);
libreCudaStatus_t kernelLaunchModuleGetFunctionCount(uint32_t *count, LibreCUmodule mod);
libreCudaStatus_t kernelLaunchModuleEnumerateFunctions(LibreCUFunction *functionsOut, uint32_t numFunctions, LibreCUmodule mod);
libreCudaStatus_t kernelLaunchModuleGetFunction(LibreCUFunction *pFunc, LibreCUmodule module, const char *name);
libreCudaStatus_t kernelLaunchModuleUnload(LibreCUmodule module);

// Function management  
libreCudaStatus_t kernelLaunchFuncGetName(const char **pNameOut, LibreCUFunction func);
libreCudaStatus_t kernelLaunchFuncSetAttribute(LibreCUFunction function, LibreCuFunctionAttribute attribute, int value);

// Kernel launching
libreCudaStatus_t kernelLaunchKernel(LibreCUFunction function,
                                    uint32_t gridDimX, uint32_t gridDimY, uint32_t gridDimZ,
                                    uint32_t blockDimX, uint32_t blockDimY, uint32_t blockDimZ,
                                    uint32_t sharedMemBytes, LibreCUstream stream,
                                    void **kernelParams, size_t numParams, void **extra, bool async = false);

#ifdef __cplusplus
}
#endif

#endif // LIBRECUDA_KERNEL_LAUNCH_H_