cmake_minimum_required(VERSION 3.26)
project(video_app)

set(CMAKE_CXX_STANDARD 20)


add_subdirectory(lib/glfw)

if (WIN32)
    list(APPEND EXTRA_LIBS
            "-lglu32 -lopengl32"
    )
    set(CMAKE_EXE_LINKER_FLAGS "-std=c++11 -static-libgcc -static-libstdc++ -static")
endif ()

include_directories(D:/zpplibrary/ffmpeg-win64-shared/include)
link_directories(D:/zpplibrary/ffmpeg-win64-shared/lib)

add_executable(video_app main.cpp
        load_frame.cpp
        video_reader.cpp
        video_reader.h)

target_link_libraries(
        video_app
        avcodec
        avdevice
        avfilter
        avformat
        avutil
        postproc
        swresample
        swscale
        mingw32
        glfw
        ${EXTRA_LIBS}
)