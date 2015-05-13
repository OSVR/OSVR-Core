if(ANDROID_CRYSTAX_NDK)
    # Remove -lc -lm -ldl from libs
    foreach(lib ${OpenCV_LIB_COMPONENTS})
        foreach(config RELEASE DEBUG)
            get_target_property(INTERFACE_LINK ${lib} IMPORTED_LINK_INTERFACE_LIBRARIES_${CONFIG})
            if(INTERFACE_LINK)
                list(REMOVE_ITEM INTERFACE_LINK c dl m)
                set_target_properties(${lib} PROPERTIES
                    IMPORTED_LINK_INTERFACE_LIBRARIES_${CONFIG} "${INTERFACE_LINK}")
            endif()
        endforeach()
    endforeach()
endif()
