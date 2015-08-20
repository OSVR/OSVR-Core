include(CMakeParseArguments) # Bundled with CMake 2.8.3 and newer

set(_embed_shader_dir "${CMAKE_CURRENT_LIST_DIR}")
function(osvr_embed_shader)
    cmake_parse_arguments(OSVR_EMBED_SHADER "" "SOURCE;DEST;SYMBOL" "" ${ARGN})
    if(NOT OSVR_EMBED_SHADER_SOURCE OR NOT OSVR_EMBED_SHADER_DEST OR NOT OSVR_EMBED_SHADER_SYMBOL)
        message(FATAL_ERROR "Must specify a SOURCE, DEST, and SYMBOL to osvr_embed_shader!")
    endif()
    add_custom_command(OUTPUT "${OSVR_EMBED_SHADER_DEST}"
        COMMAND
        "${CMAKE_COMMAND}"
        "-DDEST=${OSVR_EMBED_SHADER_DEST}"
        "-DSOURCE=${OSVR_EMBED_SHADER_SOURCE}"
        "-DSYMBOL=${OSVR_EMBED_SHADER_SYMBOL}"
        "-DTEMPLATE=${_embed_shader_dir}/EmbedShader.h.in"
        -P "${_embed_shader_dir}/EmbedShaderCmd.cmake"
        MAIN_DEPENDENCY
        "${OSVR_EMBED_SHADER_SOURCE}"
        DEPENDS
        "${_embed_shader_dir}/EmbedShader.h.in"
        "${_embed_shader_dir}/EmbedShaderCmd.cmake"
        VERBATIM
        COMMENT
        "Transforming shader into header file ${OSVR_EMBED_SHADER_DEST}")
    set_property(DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}" APPEND
        PROPERTY ADDITIONAL_MAKE_CLEAN_FILES "${OSVR_EMBED_SHADER_DEST}")
endfunction()
