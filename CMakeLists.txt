cmake_minimum_required(VERSION 3.15)
project(ag_ffi_header)

add_library(ag_ffi_header INTERFACE)

target_include_directories(ag_ffi_header
    INTERFACE
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:ag-ffi-header/include>
)
