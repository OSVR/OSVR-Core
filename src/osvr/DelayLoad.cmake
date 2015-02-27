# Delay-load the specified DLLs from OpenCV, on MSVC
function(osvr_delayload_opencv _target)
    if(MSVC)
        foreach(_dep ${ARGN})
            # Not perfectly elegant, but can't use generator expressions in LINK_FLAGS...
            set_property(TARGET ${_target} APPEND PROPERTY LINK_FLAGS "/DELAYLOAD:${_dep}${OpenCV_VERSION_MAJOR}${OpenCV_VERSION_MINOR}${OpenCV_VERSION_PATCH}.dll")
            target_link_libraries(${_target} PRIVATE DelayImp.lib)
        endforeach()
    endif()
endfunction()