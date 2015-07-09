# Delay-load the specified DLLs from OpenCV, on MSVC
function(osvr_delayload_opencv _target)
    if(MSVC)
        get_property(_flags TARGET ${_target} PROPERTY LINK_FLAGS)
        if(NOT _flags)
            set(_flags)
        endif()
        foreach(_dep ${ARGN})
            # Not perfectly elegant, but can't use generator expressions in LINK_FLAGS...
            list(APPEND _flags
                "/DELAYLOAD:${_dep}${OpenCV_VERSION_MAJOR}${OpenCV_VERSION_MINOR}${OpenCV_VERSION_PATCH}.dll"
                "/DELAYLOAD:${_dep}${OpenCV_VERSION_MAJOR}${OpenCV_VERSION_MINOR}${OpenCV_VERSION_PATCH}d.dll")
        endforeach()
        # Turn a list into space-delimited args
        string(REPLACE ";" " " _flags "${_flags}")
        set_property(TARGET ${_target} PROPERTY LINK_FLAGS "${_flags}")
        target_link_libraries(${_target} PRIVATE DelayImp.lib)
    endif()
endfunction()
