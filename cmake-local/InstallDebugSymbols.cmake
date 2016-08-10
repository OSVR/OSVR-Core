if(MSVC AND NOT CMAKE_VERSION VERSION_LESS 3.2)
    function(osvr_install_symbols_for_target _target)
        # debug symbols for MSVC: supported on CMake 3.2 and up where there's a
        # generator expression for a target's PDB file
        get_target_property(_target_type ${_target} TYPE)
        if("${_target_type}" STREQUAL "EXECUTABLE" OR "${_target_type}" STREQUAL "SHARED_LIBRARY")
            # exe or dll: put it alongside the runtime component
            set(DEST ${CMAKE_INSTALL_BINDIR})
        else()
            set(DEST ${CMAKE_INSTALL_LIBDIR})
        endif()
        set(HAS_SYMBOLS_CONDITION $<OR:$<CONFIG:RelWithDebInfo>,$<CONFIG:Debug>>)
        install(FILES $<${HAS_SYMBOLS_CONDITION}:$<TARGET_PDB_FILE:${_target}>>
            DESTINATION ${DEST}
            COMPONENT Devel
            ${ARGN})
    endfunction()
else()
    function(osvr_install_symbols_for_target)
        # do nothing if too old of CMake or not MSVC.
    endfunction()
endif()
