cmake_minimum_required(VERSION 3.15)
project(ag_ffi_header)

add_library(ag_ffi_header INTERFACE)

target_include_directories(ag_ffi_header
    INTERFACE
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:ag-ffi-header/include>
)

# Applies to the target dependency on ffi header, adds no-except, adds output dirs, copies ag and windows dlls
function(ag_ffi_configure_target ag_target ag_file_name)
    set_property(TARGET ${ag_target} PROPERTY C_STANDARD 11)
    target_link_libraries(${ag_target} PRIVATE ag_ffi_header)
    target_compile_options(${ag_target} PRIVATE
        $<$<CXX_COMPILER_ID:MSVC>:/EHs-c->
        $<$<CXX_COMPILER_ID:GNU,Clang,AppleClang>:-fno-exceptions>
    )

    set(DST "$<IF:$<BOOL:${AG_TRIPLE}>,${AG_OUT_DIR}/${AG_TRIPLE}/$<CONFIG>,${CMAKE_BINARY_DIR}>")
    set(AG_DST "$<IF:$<BOOL:${AG_TRIPLE}>,${AG_OUT_DIR},${CMAKE_BINARY_DIR}>")
    set_target_properties(${ag_target} PROPERTIES
        ARCHIVE_OUTPUT_DIRECTORY "${DST}"
        LIBRARY_OUTPUT_DIRECTORY "${DST}"
        RUNTIME_OUTPUT_DIRECTORY "${DST}"
    )

    if(ag_file_name)
        add_custom_command(TARGET ${ag_target} POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy_if_different
                    "${CMAKE_CURRENT_SOURCE_DIR}/${ag_file_name}"
                    "${AG_DST}/${ag_file_name}"
        )
    endif()

    if(WIN32)
        set(DLL_TRIPLE "${AG_TRIPLE}")
        if(NOT DLL_TRIPLE)
            set(DLL_TRIPLE "${VCPKG_TARGET_TRIPLET}")
        endif()
        if(CMAKE_BUILD_TYPE STREQUAL "Debug" OR CONFIG STREQUAL "Debug")
            set(DEBUG_BIN_DIR "${CMAKE_BINARY_DIR}/vcpkg_installed/${DLL_TRIPLE}/debug/bin")
            file(GLOB DBG_DLLS "${DEBUG_BIN_DIR}/*.dll")
            file(GLOB DBG_PDBS "${DEBUG_BIN_DIR}/*.pdb")
            foreach(f ${DBG_DLLS} ${DBG_PDBS})
                add_custom_command(TARGET ${ag_target} POST_BUILD
                    COMMAND ${CMAKE_COMMAND} -E copy_if_different "${f}" "${DST}"
                )
            endforeach()
        else()
            set(REL_BIN_DIR "${CMAKE_BINARY_DIR}/vcpkg_installed/${DLL_TRIPLE}/bin")
            file(GLOB REL_DLLS "${REL_BIN_DIR}/*.dll")
            foreach(dll ${REL_DLLS})
                add_custom_command(TARGET ${ag_target} POST_BUILD
                    COMMAND ${CMAKE_COMMAND} -E copy_if_different "${dll}" "${DST}"
                )
            endforeach()
        endif()
    endif()
endfunction()
