set(required_boost_components)
# Boost Thread
if(NEED_BOOST_THREAD)
    list(APPEND required_boost_components thread system date_time chrono)
endif()

if(NEED_BOOST_PROGRAM_OPTIONS)
    list(APPEND required_boost_components program_options)
endif()

if(NEED_BOOST_FILESYSTEM)
    list(APPEND required_boost_components filesystem)
endif()

if(NEED_BOOST_LOCALE)
    list(APPEND required_boost_components locale)
endif()

if(WIN32)
    set(OSVR_MIN_BOOST 1.54) # ABI break in Boost IPC on Windows pre 1.54
else()
    set(OSVR_MIN_BOOST 1.44) # Lower version bound of 1.43 for range adapters, 1.44 for Filesystem v3
endif()

find_package(Boost ${OSVR_MIN_BOOST} COMPONENTS ${required_boost_components} REQUIRED)

if(NEED_BOOST_THREAD)
    add_library(boost_thread INTERFACE)
    target_link_libraries(boost_thread
        INTERFACE
        ${Boost_THREAD_LIBRARY}
        ${Boost_SYSTEM_LIBRARY}
        ${Boost_DATE_TIME_LIBRARY}
        ${Boost_CHRONO_LIBRARY}
        ${CMAKE_THREAD_LIBS_INIT})
    target_include_directories(boost_thread INTERFACE ${Boost_INCLUDE_DIR})
    if(WIN32 AND NOT MSVC AND Boost_USE_STATIC_LIBS)
        # Work around a foolish insistence to use DLLs on MXE that don't exist.
        target_compile_definitions(boost_thread
            INTERFACE
            BOOST_THREAD_USE_LIB)
    endif()
endif()

if(NEED_BOOST_FILESYSTEM)
    add_library(boost_filesystem INTERFACE)
    target_link_libraries(boost_filesystem
        INTERFACE
        ${Boost_FILESYSTEM_LIBRARY}
        ${Boost_SYSTEM_LIBRARY})
    target_include_directories(boost_filesystem INTERFACE ${Boost_INCLUDE_DIR})
    target_compile_definitions(boost_filesystem INTERFACE BOOST_FILESYSTEM_VERSION=3)
endif()

if(NEED_BOOST_PROGRAM_OPTIONS)
    add_library(boost_program_options INTERFACE)
    target_link_libraries(boost_program_options
        INTERFACE
        ${Boost_PROGRAM_OPTIONS_LIBRARY})
    target_include_directories(boost_program_options INTERFACE ${Boost_INCLUDE_DIR})
endif()

if(NEED_BOOST_LOCALE)
    add_library(boost_locale INTERFACE)
    target_link_libraries(boost_locale
        INTERFACE
        ${Boost_LOCALE_LIBRARY})
    target_include_directories(boost_locale INTERFACE ${Boost_INCLUDE_DIR})
endif()
