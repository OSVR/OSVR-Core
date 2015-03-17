
# Call with the output var, then with one or more dependencies (imported targets)
# It will APPEND directories to look in to your output var.
function(get_search_dirs _var)
    set(properties_to_check LOCATION)
    set(configs ${CMAKE_CONFIGURATION_TYPES})
    if(CMAKE_BUILD_TYPE)
        # NOCONFIG is the name used in exported target files when CMAKE_BUILD_TYPE
        # is left blank.
        list(APPEND configs ${CMAKE_BUILD_TYPE} NOCONFIG)
    endif()

    if(configs)
        foreach(config ${configs})
            string(TOUPPER ${config} config_uc)
            list(APPEND properties_to_check LOCATION_${config_uc})
        endforeach()
    endif()

    set(searchdirs ${${_var}})
    foreach(dep ${ARGN})
        if(TARGET ${dep})
            foreach(prop ${properties_to_check})
                get_property(loc_set TARGET ${dep} PROPERTY ${prop} SET)
                if(loc_set)
                    get_property(loc TARGET ${dep} PROPERTY ${prop})
                    get_filename_component(dir "${loc}" DIRECTORY)
                    list(APPEND searchdirs "${dir}")
                endif()
            endforeach()
        endif()
    endforeach()

    if(searchdirs)
        list(REMOVE_DUPLICATES searchdirs)
    endif()
    set(${_var} ${searchdirs} PARENT_SCOPE)
endfunction()
