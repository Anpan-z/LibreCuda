set(
        DRIVERAPI_SOURCES
        src/librecuda.cpp
        src/librecuda_status.cpp
        src/cmdqueue.cpp
        src/memcopy.cpp
        modules/rm_ioctl/rm_ioctl.cpp
        modules/uvm_ioctl/uvm_ioctl.cpp
        internal/memcopy.h
        internal/memcopy_kernels.h
)
if (BUILD_LIBRECUDA_DRIVER_API_STATIC_LIB)
    add_library(driverapi STATIC ${DRIVERAPI_SOURCES})
else()
    add_library(driverapi SHARED ${DRIVERAPI_SOURCES})
endif()

target_include_directories(driverapi PRIVATE "internal/")
target_include_directories(driverapi PRIVATE "modules/rm_ioctl/")
target_include_directories(driverapi PRIVATE "modules/uvm_ioctl/")
target_include_directories(driverapi PRIVATE "modules/device_management/")
target_include_directories(driverapi PRIVATE "modules/context_management/")
target_include_directories(driverapi PRIVATE "modules/memory_management/")
target_include_directories(driverapi PRIVATE "modules/stream_management/")
target_include_directories(driverapi PRIVATE "modules/kernel_launch/")
target_include_directories(driverapi PUBLIC "include/")
target_link_libraries(driverapi PRIVATE elfio)
