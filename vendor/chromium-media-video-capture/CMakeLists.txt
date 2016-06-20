include_directories("${CMAKE_CURRENT_SOURCE_DIR}")

set(SOURCES
    base/logging.h
    base/memory/ref_counted.cc
    base/memory/ref_counted.h
    media/base/video_frame.cc
    media/base/video_frame.h
    media/video/capture/video_capture.h
    media/video/capture/video_capture_device.h
    media/video/capture/video_capture_types.h
)

if(WIN32)
    list(APPEND SOURCES
        base/win/scoped_comptr.h
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
    target_compile_definitions(chromium-video-capture PUBLIC OS_WIN)
endif()
if(MSVC)
    target_compile_definitions(chromium-video-capture PUBLIC COMPILER_MSVC)
endif()

target_compile_definitions(chromium-video-capture PUBLIC BASE_EXPORT MEDIA_EXPORT)
