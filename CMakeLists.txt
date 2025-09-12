cmake_minimum_required(VERSION 3.15)
project(ag-ffi-heder LANGUAGES CXX)

add_library(ag-ffi-heder INTERFACE)

target_include_directories(ag-ffi-heder
    INTERFACE
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include>
)

target_compile_features(ag-ffi-heder INTERFACE cxx_std_17)
