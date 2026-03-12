#include <librecuda.h>
#include "../../driverapi/internal/librecuda_internal.h"
#include "../../driverapi/internal/nvidia/nv-unix-nvos-params-wrappers.h"
#include "../../driverapi/internal/nvidia/nv_escape.h"
#include "../../driverapi/internal/nvidia/g_allclasses.h"
#include "../../driverapi/internal/nvidia/uvm_ioctl.h"

#include <cassert>
#include <cerrno>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <string>
#include <unordered_map>
#include <vector>

namespace {

constexpr int kFakeDeviceFd = 21;
constexpr int kFakeCtlFd = 11;
constexpr int kFakeUvmFd = 12;
constexpr NvHandle kFakeRootHandle = 0xc1d98e38;
constexpr NvHandle kFakeDeviceHandle = 0x5c000002;
constexpr NvHandle kFakeMemoryHandle = 0x5c000088;
constexpr size_t kHostAllocGranularity = 2UL * 1024UL * 1024UL;

enum class FailurePoint {
    None,
    RmAllocMemory,
    UvmMapExternalAllocation,
};

FailurePoint g_failure_point = FailurePoint::None;
std::vector<std::string> g_call_log{};
std::unordered_map<void *, size_t> g_mapped_regions{};

unsigned long makeNvRequest(unsigned int nr, size_t dataSize) {
    return (3UL << 30) | ((dataSize & 0x1FFFUL) << 16) | (static_cast<unsigned long>('F') << 8) | (nr & 0xFFUL);
}

void resetHooks() {
    g_failure_point = FailurePoint::None;
    g_call_log.clear();
    for (const auto &entry: g_mapped_regions) {
        std::free(entry.first);
    }
    g_mapped_regions.clear();
}

void expect(bool condition, const std::string &message) {
    if (!condition) {
        std::cerr << "[TEST FAILURE] " << message << std::endl;
        std::exit(EXIT_FAILURE);
    }
}

LibreCUcontext makeFakeContext(LibreCUdevice_ &device, LibreCUcontext_ &ctx) {
    std::memset(device.uuid.uuid, 0xA4, sizeof(device.uuid.uuid));
    device.instance = 0;
    ctx.device = &device;
    ctx.device_handle = kFakeDeviceHandle;
    ctx.device_fd = kFakeDeviceFd;
    return &ctx;
}

void verifySuccessOrder() {
    const std::vector<std::string> expected{
        "mmap",
        "rm_alloc_memory",
        "uvm_create_external_range",
        "uvm_map_external_allocation",
        "uvm_unmap_external",
        "uvm_free",
        "rm_free",
        "munmap"
    };
    expect(g_call_log == expected, "unexpected host alloc/free ioctl sequence");
}

void testInvalidParameters() {
    void *host_ptr = reinterpret_cast<void *>(0x1);
    expect(libreCuMemAllocHost(nullptr, 4096) == LIBRECUDA_ERROR_INVALID_VALUE, "nullptr output should fail");
    expect(libreCuMemAllocHost(&host_ptr, 0) == LIBRECUDA_ERROR_INVALID_VALUE, "zero-sized alloc should fail");
    expect(libreCuMemFreeHost(nullptr) == LIBRECUDA_ERROR_INVALID_VALUE, "nullptr free should fail");
}

void testSuccessfulAllocAndFree() {
    LibreCUdevice_ device{};
    LibreCUcontext_ ctx{};
    libreCudaTestConfigureHostAllocRuntime(makeFakeContext(device, ctx), kFakeRootHandle, kFakeCtlFd, kFakeUvmFd);

    void *host_ptr = nullptr;
    expect(libreCuMemAllocHost(&host_ptr, 4096) == LIBRECUDA_SUCCESS, "host alloc should succeed");
    expect(host_ptr != nullptr, "host alloc should write pointer");

    auto *bytes = reinterpret_cast<unsigned char *>(host_ptr);
    for (size_t i = 0; i < 4096; i++) {
        bytes[i] = static_cast<unsigned char>(i & 0xFF);
    }
    for (size_t i = 0; i < 4096; i++) {
        expect(bytes[i] == static_cast<unsigned char>(i & 0xFF), "host allocation should be writable/readable");
    }

    expect(libreCuMemFreeHost(host_ptr) == LIBRECUDA_SUCCESS, "host free should succeed");
    expect(libreCuMemFreeHost(host_ptr) == LIBRECUDA_ERROR_INVALID_VALUE, "double free should be rejected");
    expect(libreCuMemFreeHost(reinterpret_cast<void *>(0x1234)) == LIBRECUDA_ERROR_INVALID_VALUE,
           "freeing unknown pointer should be rejected");

    verifySuccessOrder();
    libreCudaTestResetHostAllocRuntime();
}

void testRmAllocFailure() {
    LibreCUdevice_ device{};
    LibreCUcontext_ ctx{};
    libreCudaTestConfigureHostAllocRuntime(makeFakeContext(device, ctx), kFakeRootHandle, kFakeCtlFd, kFakeUvmFd);

    g_failure_point = FailurePoint::RmAllocMemory;
    void *host_ptr = nullptr;
    expect(libreCuMemAllocHost(&host_ptr, 4096) == LIBRECUDA_ERROR_OUT_OF_MEMORY,
           "ENOMEM from RM alloc should map to out-of-memory");
    expect(host_ptr == nullptr, "failed alloc should leave output untouched");
    expect(g_call_log == std::vector<std::string>({"mmap", "rm_alloc_memory", "munmap"}),
           "rm alloc failure should munmap reserved host range");

    libreCudaTestResetHostAllocRuntime();
}

void testUvmMapFailure() {
    LibreCUdevice_ device{};
    LibreCUcontext_ ctx{};
    libreCudaTestConfigureHostAllocRuntime(makeFakeContext(device, ctx), kFakeRootHandle, kFakeCtlFd, kFakeUvmFd);

    g_failure_point = FailurePoint::UvmMapExternalAllocation;
    void *host_ptr = nullptr;
    expect(libreCuMemAllocHost(&host_ptr, 4096) == LIBRECUDA_ERROR_UNKNOWN,
           "uvm map failure should propagate as unknown");
    expect(host_ptr == nullptr, "failed alloc should not expose host pointer");
    expect(g_call_log == std::vector<std::string>({
        "mmap",
        "rm_alloc_memory",
        "uvm_create_external_range",
        "uvm_map_external_allocation",
        "uvm_free",
        "rm_free",
        "munmap"
    }), "uvm map failure should release all intermediate resources");

    libreCudaTestResetHostAllocRuntime();
}

} // namespace

extern "C" int libreCudaTestHostAllocIoctl(int fd, unsigned long request, void *data) {
    if (request == makeNvRequest(NV_ESC_RM_ALLOC_MEMORY, sizeof(nv_ioctl_nvos02_parameters_with_fd))) {
        g_call_log.emplace_back("rm_alloc_memory");
        expect(fd == kFakeDeviceFd, "RM alloc memory should use the device fd from context");
        auto *params = reinterpret_cast<nv_ioctl_nvos02_parameters_with_fd *>(data);
        expect(params->params.hRoot == kFakeRootHandle, "RM alloc should use configured root handle");
        expect(params->params.hObjectParent == kFakeDeviceHandle, "RM alloc should use current device handle");
        expect(params->params.hClass == NV01_MEMORY_SYSTEM_OS_DESCRIPTOR, "RM alloc should use system OS descriptor");
        expect(params->params.flags == 0x40001010, "RM alloc flags should match the trace");
        expect(params->params.limit == (kHostAllocGranularity - 1), "RM alloc should round to 2 MiB");
        expect(params->fd == -1, "RM alloc should pass fd=-1");
        if (g_failure_point == FailurePoint::RmAllocMemory) {
            errno = ENOMEM;
            return -1;
        }
        params->params.hObjectNew = kFakeMemoryHandle;
        params->params.status = 0;
        return 0;
    }

    if (request == UVM_CREATE_EXTERNAL_RANGE) {
        g_call_log.emplace_back("uvm_create_external_range");
        expect(fd == kFakeUvmFd, "UVM create external range should use the UVM fd");
        auto *params = reinterpret_cast<UVM_CREATE_EXTERNAL_RANGE_PARAMS *>(data);
        expect(params->length == kHostAllocGranularity, "external range length should match rounded allocation");
        params->rmStatus = 0;
        return 0;
    }

    if (request == UVM_MAP_EXTERNAL_ALLOCATION) {
        g_call_log.emplace_back("uvm_map_external_allocation");
        expect(fd == kFakeUvmFd, "UVM external mapping should use the UVM fd");
        auto *params = reinterpret_cast<UVM_MAP_EXTERNAL_ALLOCATION_PARAMS *>(data);
        expect(params->length == kHostAllocGranularity, "UVM map length should match rounded allocation");
        expect(params->offset == 0, "UVM map offset should be zero");
        expect(params->gpuAttributesCount == 1, "UVM map should register a single GPU mapping attribute");
        expect(params->rmCtrlFd == kFakeCtlFd, "UVM map should use the configured rm control fd");
        expect(params->hClient == kFakeRootHandle, "UVM map should use the configured root handle");
        expect(params->hMemory == kFakeMemoryHandle, "UVM map should use the RM allocation handle");
        if (g_failure_point == FailurePoint::UvmMapExternalAllocation) {
            params->rmStatus = -1;
            return 0;
        }
        params->rmStatus = 0;
        return 0;
    }

    if (request == UVM_UNMAP_EXTERNAL) {
        g_call_log.emplace_back("uvm_unmap_external");
        auto *params = reinterpret_cast<UVM_UNMAP_EXTERNAL_PARAMS *>(data);
        expect(params->length == kHostAllocGranularity, "UVM unmap length should match rounded allocation");
        params->rmStatus = 0;
        return 0;
    }

    if (request == UVM_FREE) {
        g_call_log.emplace_back("uvm_free");
        auto *params = reinterpret_cast<UVM_FREE_PARAMS *>(data);
        expect(params->length == kHostAllocGranularity, "UVM free length should match rounded allocation");
        params->rmStatus = 0;
        return 0;
    }

    if (request == makeNvRequest(NV_ESC_RM_FREE, sizeof(NVOS00_PARAMETERS))) {
        g_call_log.emplace_back("rm_free");
        expect(fd == kFakeCtlFd, "RM free should use the configured control fd");
        auto *params = reinterpret_cast<NVOS00_PARAMETERS *>(data);
        expect(params->hRoot == kFakeRootHandle, "RM free should use configured root handle");
        expect(params->hObjectParent == kFakeDeviceHandle, "RM free should use current device handle");
        expect(params->hObjectOld == kFakeMemoryHandle, "RM free should use the allocated memory handle");
        params->status = 0;
        return 0;
    }

    std::cerr << "unexpected ioctl request: " << request << std::endl;
    std::exit(EXIT_FAILURE);
}

extern "C" void *libreCudaTestHostAllocMmap(void *addr, size_t length, int prot, int flags, int fd, off_t offset) {
    (void) addr;
    (void) prot;
    (void) flags;
    (void) fd;
    (void) offset;
    g_call_log.emplace_back("mmap");
    auto *memory = static_cast<unsigned char *>(std::malloc(length));
    expect(memory != nullptr, "test mmap malloc should succeed");
    std::memset(memory, 0, length);
    g_mapped_regions[memory] = length;
    return memory;
}

extern "C" int libreCudaTestHostAllocMunmap(void *addr, size_t length) {
    g_call_log.emplace_back("munmap");
    auto it = g_mapped_regions.find(addr);
    expect(it != g_mapped_regions.end(), "munmap should receive a tracked region");
    expect(it->second == length, "munmap length should match tracked region");
    std::free(it->first);
    g_mapped_regions.erase(it);
    return 0;
}

int main() {
    resetHooks();
    testInvalidParameters();

    resetHooks();
    testSuccessfulAllocAndFree();

    resetHooks();
    testRmAllocFailure();

    resetHooks();
    testUvmMapFailure();

    resetHooks();
    std::cout << "host_alloc tests passed" << std::endl;
    return 0;
}
