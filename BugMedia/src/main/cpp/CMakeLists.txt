# For more information about using CMake with Android Studio, read the
# documentation: https://d.android.com/studio/projects/add-native-code.html

# Sets the minimum version of CMake required to build the native library.

cmake_minimum_required(VERSION 3.10.2)

# Declares and names the project.

project("bugmedia")

#set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR})

# jni目录配置，不同的架构对应不同的目录
set(jniLibs_dir ${CMAKE_CURRENT_SOURCE_DIR}/../../../jniLibs)
set(libs_dir ${jniLibs_dir}/${CMAKE_ANDROID_ARCH_ABI})
# 头文件所在目录
set(base_include_dir ${jniLibs_dir}/include)


# ffmpeg
include_directories(${base_include_dir}/ffmpeg)
set(ffmpeg_lib_dir ${libs_dir}/ffmpeg)


add_library(avutil
        SHARED
        IMPORTED)
set_target_properties(avutil
        PROPERTIES IMPORTED_LOCATION
        ${ffmpeg_lib_dir}/libavutil.so)

add_library(swresample
        SHARED
        IMPORTED)
set_target_properties(swresample
        PROPERTIES IMPORTED_LOCATION
        ${ffmpeg_lib_dir}/libswresample.so)

add_library(avcodec
        SHARED
        IMPORTED)
set_target_properties(avcodec
        PROPERTIES IMPORTED_LOCATION
        ${ffmpeg_lib_dir}/libavcodec.so)

add_library(avfilter
        SHARED
        IMPORTED)
set_target_properties(avfilter
        PROPERTIES IMPORTED_LOCATION
        ${ffmpeg_lib_dir}/libavfilter.so)

add_library(swscale
        SHARED
        IMPORTED)
set_target_properties(swscale
        PROPERTIES IMPORTED_LOCATION
        ${ffmpeg_lib_dir}/libswscale.so)

add_library(avformat
        SHARED
        IMPORTED)
set_target_properties(avformat
        PROPERTIES IMPORTED_LOCATION
        ${ffmpeg_lib_dir}/libavformat.so)

add_library(avdevice
        SHARED
        IMPORTED)
set_target_properties(avdevice
        PROPERTIES IMPORTED_LOCATION
        ${ffmpeg_lib_dir}/libavdevice.so)

add_library(
        postproc
        SHARED
        IMPORTED
)
SET_TARGET_PROPERTIES(
        postproc
        PROPERTIES IMPORTED_LOCATION
        ${ffmpeg_lib_dir}/libpostproc.so
)


# Creates and names a library, sets it as either STATIC
# or SHARED, and provides the relative paths to its source code.
# You can define multiple libraries, and CMake builds them for you.
# Gradle automatically packages shared libraries with your APK.

add_library( # Sets the name of the library.
        bugmedia

        # Sets the library as a shared library.
        SHARED

        # Provides a relative path to your source file(s).
        EGL/BugMediaEGL.cpp
        OpenGLES/BugMediaGLES.cpp
        BugMediaVideoRenderer.cpp
        BugMediaBridge.cpp
        BugMediaPlayer.cpp
        BugMediaAudioFrame.cpp
        BugMediaVideoFrame.cpp
        BugMediaCommon.cpp
        BugMediaFFmpegDecoder.cpp
        BugMediaSLESAudioRenderer.cpp

        )


# Searches for a specified prebuilt library and stores the path as a
# variable. Because CMake includes system libraries in the search path by
# default, you only need to specify the name of the public NDK library
# you want to add. CMake verifies that the library exists before
# completing its build.

find_library( # Sets the name of the path variable.
        log-lib

        # Specifies the name of the NDK library that
        # you want CMake to locate.
        log)

find_library(
        android-lib
        android
)


# Specifies libraries CMake should link to your target library. You
# can link multiple libraries, such as libraries you define in this
# build script, prebuilt third-party libraries, or system libraries.

target_link_libraries( # Specifies the target library.
        bugmedia

        EGL
        GLESv2
        OpenSLES
        avutil
        swresample
        avcodec
        avfilter
        swscale
        avformat
        avdevice
        postproc
        ${android-lib}
        ${log-lib})