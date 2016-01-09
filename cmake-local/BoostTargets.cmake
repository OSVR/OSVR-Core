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

find_package(Boost 1.44 COMPONENTS ${required_boost_components} REQUIRED) # Lower version bound of 1.43 for range adapters, 1.44 for Filesystem v3
if(Boost_VERSION GREATER 105900)
    message(SEND_ERROR "Using an untested Boost version - inspect the Boost Interprocess release notes/changelog to see if any ABI breaks affect us.")
endif(Boost_VERSION GREATER 105900)

if(NEED_BOOST_THREAD)
    add_library(boost_thread INTERFACE)
    target_link_libraries(boost_thread
        INTERFACE
        ${Boost_THREAD_LIBRARIES}
        ${Boost_SYSTEM_LIBRARIES}
        ${Boost_DATE_TIME_LIBRARIES}
        ${Boost_CHRONO_LIBRARIES}
        ${CMAKE_THREAD_LIBS_INIT})
    target_include_directories(boost_thread INTERFACE ${Boost_INCLUDE_DIRS})
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
        ${Boost_FILESYSTEM_LIBRARIES}
        ${Boost_SYSTEM_LIBRARIES})
    target_include_directories(boost_filesystem INTERFACE ${Boost_INCLUDE_DIRS})
    target_compile_definitions(boost_filesystem INTERFACE BOOST_FILESYSTEM_VERSION=3)
endif()

if(NEED_BOOST_PROGRAM_OPTIONS)
    add_library(boost_program_options INTERFACE)
    target_link_libraries(boost_program_options
        INTERFACE
        ${Boost_PROGRAM_OPTIONS_LIBRARIES})
    target_include_directories(boost_program_options INTERFACE ${Boost_INCLUDE_DIRS})
endif()

if(NEED_BOOST_LOCALE)
    add_library(boost_locale INTERFACE)
    target_link_libraries(boost_locale
        INTERFACE
    ${Boost_LOCALE_LIBRARIES})
    target_include_directories(boost_locale INTERFACE ${Boost_INCLUDE_DIRS})
endif()
