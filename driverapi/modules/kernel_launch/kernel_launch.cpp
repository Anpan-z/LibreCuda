#include "kernel_launch.h"
#include "librecuda.h"

// These are wrapper functions that call the existing librecuda functions
// Eventually, we'll move the actual implementation here

libreCudaStatus_t kernelLaunchModuleLoadData(LibreCUmodule *pModuleOut, const void *image, size_t imageSize) {
    return libreCuModuleLoadData(pModuleOut, image, imageSize);
}

libreCudaStatus_t kernelLaunchModuleGetFunctionCount(uint32_t *count, LibreCUmodule mod) {
    return libreCuModuleGetFunctionCount(count, mod);
}

libreCudaStatus_t kernelLaunchModuleEnumerateFunctions(LibreCUFunction *functionsOut, uint32_t numFunctions, LibreCUmodule mod) {
    return libreCuModuleEnumerateFunctions(functionsOut, numFunctions, mod);
}

libreCudaStatus_t kernelLaunchModuleGetFunction(LibreCUFunction *pFunc, LibreCUmodule module, const char *name) {
    return libreCuModuleGetFunction(pFunc, module, name);
}

libreCudaStatus_t kernelLaunchModuleUnload(LibreCUmodule module) {
    return libreCuModuleUnload(module);
}

libreCudaStatus_t kernelLaunchFuncGetName(const char **pNameOut, LibreCUFunction func) {
    return libreCuFuncGetName(pNameOut, func);
}

libreCudaStatus_t kernelLaunchFuncSetAttribute(LibreCUFunction function, LibreCuFunctionAttribute attribute, int value) {
    return libreCuFuncSetAttribute(function, attribute, value);
}

libreCudaStatus_t kernelLaunchKernel(LibreCUFunction function,
                                    uint32_t gridDimX, uint32_t gridDimY, uint32_t gridDimZ,
                                    uint32_t blockDimX, uint32_t blockDimY, uint32_t blockDimZ,
                                    uint32_t sharedMemBytes, LibreCUstream stream,
                                    void **kernelParams, size_t numParams, void **extra, bool async) {
    return libreCuLaunchKernel(function, gridDimX, gridDimY, gridDimZ, blockDimX, blockDimY, blockDimZ,
                               sharedMemBytes, stream, kernelParams, numParams, extra, async);
}