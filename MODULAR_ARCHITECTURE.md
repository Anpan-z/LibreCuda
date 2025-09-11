# LibreCuda Modular Architecture

This document describes the new modular architecture of LibreCuda, which organizes functionality into five main modules plus ioctl encapsulation layers.

## Module Structure

### Core Modules

#### 1. Device Management (`modules/device_management/`)
- **Purpose**: Handle device enumeration, properties, and initialization
- **Key Functions**:
  - `deviceManagementInit()` - Initialize device subsystem
  - `deviceManagementGetCount()` - Get number of available devices
  - `deviceManagementGet()` - Get device handle by ordinal
  - `deviceManagementGetName()` - Get device name
  - `deviceManagementGetAttribute()` - Get device attributes

#### 2. Context Management (`modules/context_management/`)
- **Purpose**: Handle CUDA context creation, destruction, and management
- **Key Functions**:
  - `contextManagementCreate()` - Create CUDA context
  - `contextManagementDestroy()` - Destroy CUDA context
  - `contextManagementSetCurrent()` - Set current context
  - `contextManagementGetCurrent()` - Get current context

#### 3. Memory Management (`modules/memory_management/`)
- **Purpose**: Handle memory allocation, deallocation, and copying operations
- **Key Functions**:
  - `memoryManagementAlloc()` - Allocate GPU memory
  - `memoryManagementFree()` - Free GPU memory
  - `memoryManagementCopy()` - Copy memory (host-device, device-device)

#### 4. Stream Management (`modules/stream_management/`)
- **Purpose**: Handle stream creation, destruction, synchronization, and events
- **Key Functions**:
  - `streamManagementCreate()` - Create CUDA stream
  - `streamManagementDestroy()` - Destroy CUDA stream
  - `streamManagementCommence()` - Submit stream operations
  - `streamManagementAwait()` - Wait for stream completion
  - Event management functions for timing and synchronization

#### 5. Kernel Launch (`modules/kernel_launch/`)
- **Purpose**: Handle module loading, function management, and kernel launching
- **Key Functions**:
  - `kernelLaunchModuleLoadData()` - Load CUDA module from ELF binary
  - `kernelLaunchModuleGetFunction()` - Get function from module
  - `kernelLaunchKernel()` - Launch CUDA kernel
  - Function attribute management

### IOCTL Encapsulation Modules

#### 6. RM IOCTL (`modules/rm_ioctl/`)
- **Purpose**: Encapsulate Resource Manager ioctl operations
- **Key Functions**:
  - `rmCtrlCall()` - Execute RM control operations
  - `rmInit()` - Initialize RM subsystem
  - `rmGetFd()` - Get RM file descriptor
  - `rmGetRootClient()` - Get root client handle

#### 7. UVM IOCTL (`modules/uvm_ioctl/`)
- **Purpose**: Encapsulate Unified Virtual Memory ioctl operations
- **Key Functions**:
  - `uvmInit()` - Initialize UVM subsystem
  - `uvmInitializeMM()` - Initialize UVM memory management
  - `uvmGetFd()` - Get UVM file descriptor

## Design Principles

### Separation of Concerns
Each module has a single, well-defined responsibility:
- Device management handles device discovery and properties
- Context management handles CUDA context lifecycle
- Memory management handles all memory operations
- Stream management handles execution streams and events
- Kernel launch handles module loading and kernel execution
- RM/UVM ioctl modules isolate low-level hardware communication

### API Compatibility
The original LibreCuda API remains unchanged. All existing functions like `libreCuInit()`, `libreCuMemAlloc()`, etc. continue to work exactly as before. The modules provide additional internal organization and alternative APIs for more granular control.

### Incremental Adoption
The modular structure is designed to be adopted incrementally:
1. **Phase 1 (Complete)**: Module structure created with wrapper functions
2. **Phase 2 (Future)**: Gradually move implementation from monolithic `librecuda.cpp` to modules
3. **Phase 3 (Future)**: Optimize and enhance individual modules independently

## Build System Integration

The modular structure is fully integrated into the CMake build system:

```cmake
# All modules are automatically included
set(DRIVERAPI_SOURCES
    # Original files
    src/librecuda.cpp
    src/librecuda_status.cpp
    src/cmdqueue.cpp
    src/memcopy.cpp
    
    # New modular files
    modules/rm_ioctl/rm_ioctl.cpp
    modules/uvm_ioctl/uvm_ioctl.cpp
    modules/device_management/device_management.cpp
    modules/context_management/context_management.cpp
    modules/memory_management/memory_management.cpp
    modules/stream_management/stream_management.cpp
    modules/kernel_launch/kernel_launch.cpp
)
```

## Benefits

1. **Maintainability**: Code is organized by functionality, making it easier to find and modify specific features
2. **Readability**: Each module has a clear purpose and smaller, more focused codebase
3. **Testability**: Individual modules can be tested in isolation
4. **Contribution**: New contributors can focus on specific areas of functionality
5. **Documentation**: Easier to document and understand individual components
6. **IOCTL Separation**: Clear separation between RM and UVM ioctl operations

## Future Work

- Move actual implementation from `librecuda.cpp` to respective modules
- Add module-specific error handling and validation
- Create module-specific documentation and examples
- Implement module-level optimizations
- Add comprehensive unit tests for each module

## Compatibility Note

This refactoring maintains 100% API compatibility. All existing code will continue to work without any changes required.