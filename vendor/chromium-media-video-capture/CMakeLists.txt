include_directories("${CMAKE_CURRENT_SOURCE_DIR}")

set(SOURCES
    base/atomic_ref_count.h
    base/atomicops.h
    base/atomicops_internals_atomicword_compat.h
    base/atomicops_internals_mac.h
    base/atomicops_internals_x86_gcc.h
    base/atomicops_internals_x86_msvc.h
    base/basictypes.h
    base/logging.h
    base/port.h
    base/sys_string_conversions.h
    base/memory/ref_counted.cc
    base/memory/ref_counted.h
    #media/base/video_frame.cc
    #media/base/video_frame.h
    media/video/capture/video_capture.h
    media/video/capture/video_capture_device.h
    media/video/capture/video_capture_types.h
)

if(WIN32)
    list(APPEND SOURCES
        base/sys_string_conversions_win.cc
        base/string16.h
        base/win/scoped_comptr.h
        base/win/scoped_variant.cc
        base/win/scoped_variant.h
        media/video/capture/win/filter_base_win.cc
        media/video/capture/win/filter_base_win.h
        media/video/capture/win/pin_base_win.cc
        media/video/capture/win/pin_base_win.h
        media/video/capture/win/sink_filter_observer_win.h
        media/video/capture/win/sink_filter_win.cc
        media/video/capture/win/sink_filter_win.h
        media/video/capture/win/sink_input_pin_win.cc
        media/video/capture/win/sink_input_pin_win.h
        media/video/capture/win/video_capture_device_win.cc
        media/video/capture/win/video_capture_device_win.h)
endif()


add_library(chromium-video-capture STATIC ${SOURCES})

target_include_directories(chromium-video-capture PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}" "${CMAKE_CURRENT_SOURCE_DIR}/media")
if(WIN32)
#    target_compile_definitions(chromium-video-capture PUBLIC OS_WIN)
endif()
if(MSVC)
#    target_compile_definitions(chromium-video-capture PUBLIC COMPILER_MSVC)
endif()

#target_compile_definitions(chromium-video-capture PUBLIC BASE_EXPORT MEDIA_EXPORT)
target_link_libraries(chromium-video-capture PUBLIC osvrUtil)
