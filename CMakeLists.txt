cmake_minimum_required(VERSION 2.8)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -O2 -pipe")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O2 -pipe")

project(filters)

include_directories(/usr/include/opencv4)

add_executable(filters
    main.cpp
    application.cpp
    camera.cpp
    device.cpp
    surface.cpp
    pipeline.cpp
    filter.cpp
    timer.cpp

    gpu/program.cpp
    gpu/convert.cpp

    filters/backgroundsubtraction.cpp
    filters/gaussianblur.cpp
    filters/sobeloperator.cpp

    filters/backgroundsubtraction/prepare.cpp
    filters/backgroundsubtraction/draw.cpp
    filters/backgroundsubtraction/update.cpp

    filters/gaussianblur/linearblur.cpp

    filters/sobeloperator/gradient.cpp
    filters/sobeloperator/blend.cpp


    eglut/eglut.c
    eglut/eglut_x11.c
    )

target_link_libraries(filters
    X11 EGL GLESv2

    opencv_videoio
    opencv_imgproc
    opencv_core
    opencv_highgui
    opencv_video
    )
