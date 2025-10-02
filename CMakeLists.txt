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

    if(CMAKE_BUILD_TYPE STREQUAL "Release")
        set(bin_dir "${CMAKE_BINARY_DIR}/vcpkg_installed/${AG_TRIPLE}/bin")
        set(lib_dir "${CMAKE_BINARY_DIR}/vcpkg_installed/${AG_TRIPLE}/lib")
    else() # Debug
        set(bin_dir "${CMAKE_BINARY_DIR}/vcpkg_installed/${AG_TRIPLE}/debug/bin")
        set(lib_dir "${CMAKE_BINARY_DIR}/vcpkg_installed/${AG_TRIPLE}/debug/lib")
    endif()
    file(GLOB dlls "${bin_dir}/*.dll" "${bin_dir}/*.so")
    file(GLOB pdbs "${bin_dir}/*.pdb")
    file(GLOB libs "${lib_dir}/*.lib" "${lib_dir}/*.a")
    set(artifacts ${dlls} ${pdbs} ${libs})
    foreach(f IN LISTS artifacts)
        add_custom_command(TARGET ${ag_target} POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy_if_different ${f} ${DST}
        )
    endforeach()
endfunction()
