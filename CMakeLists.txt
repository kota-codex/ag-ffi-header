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

    set(seen_targets "")
    set(result_files "")
    function(_collect_recursive tgt)
        if (tgt IN_LIST seen_targets)
            return()
        endif()
        list(APPEND seen_targets "${tgt}")
        list(APPEND result_files $<TARGET_FILE:${tgt}>)
        list(APPEND result_files $<TARGET_IMPLIB:${tgt}>)
        list(APPEND result_files $<TARGET_PDB_FILE:${tgt}>)
        get_target_property(_libs ${tgt} INTERFACE_LINK_LIBRARIES)
        if (_libs)
            foreach(dep IN LISTS _libs)
                if (TARGET ${dep})
                    _collect_recursive(${dep})
                endif()
            endforeach()
        endif()
    endfunction()

    _collect_recursive(${ag_target})
    list(REMOVE_DUPLICATES result_files)
    foreach(f IN LISTS result_files)
        add_custom_command(TARGET ${ag_target} POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy_if_different "${f}" "${DST}"
        )
    endforeach()
endfunction()
