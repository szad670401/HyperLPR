# For more information about using CMake with Android Studio, read the
# documentation: https://d.android.com/studio/projects/add-native-code.html

# Sets the minimum version of CMake required to build the native library.

cmake_minimum_required(VERSION 3.4.1)
#set(OpenCV_DIR /Users/mac02/Downloads/OpenCV-android/sdk/native/jni)
set(OpenCV_DIR ${CMAKE_CURRENT_SOURCE_DIR}/opencv342/sdk/native/jni)
#message(STATUS "路径:${CMAKE_CURRENT_SOURCE_DIR}/opencvnative3_4_2/jni}")
find_package(OpenCV REQUIRED)
message(STATUS "OpenCV libraries: ${OpenCV_LIBS}")

#include_directories(/Users/mac02/Desktop/studiospace/test/PrjAnndroid/app/src/main/cpp/include)
#set(pathToOpenCv /Users/mac02/Downloads/OpenCV-android)
#配置加载native依赖
#include_directories(${pathToOpenCv}/sdk/native/jni/include)
#动态方式加载
#add_library( lib_opencv SHARED IMPORTED )
#引入libopencv_java3.so文件
#set_target_properties(lib_opencv PROPERTIES IMPORTED_LOCATION /Users/mac02/Desktop/studiospace/test/PrjAnndroid/app/src/main/jniLibs/armeabi-v7a/libopencv_java3.so)


aux_source_directory(. SOURCES1)
aux_source_directory(./src SOURCES2)
list (APPEND SOURCES
     ${SOURCES1}
     ${SOURCES2}
)
message(STATUS "My sources: ${SOURCES}")
add_library( # Sets the name of the library.
             hyperlpr

             # Sets the library as a shared library.
             SHARED

             # Provides a relative path to your source file(s).
            ${SOURCES})


#target_link_libraries( hyperlpr lib_opencv)
target_link_libraries(hyperlpr jnigraphics ${OpenCV_LIBS})

#连接现成的第三方库
#set(INC_DIR /Users/mac02/Desktop/studiospace/test/PrjAnndroid/app/src/main/cpp/OpencvNative/include)
#set(LINK_DIR /Users/mac02/Desktop/studiospace/test/PrjAnndroid/app/src/main/cpp/OpencvNative/libs)
#include_directories(OpencvNative/jni/include)

#include_directories(${INC_DIR}) # 用${}引用变量
#link_directories(${LINK_DIR})

#aux_source_directory(./include SOURCES1)
#aux_source_directory(./src SOURCES2)
#list (APPEND SOURCES
#     ${SOURCES1}
#     ${SOURCES2}
#    javaWarpper.cpp
#)
#add_library( # Sets the name of the library.
#             hyperlpr

#            # Sets the library as a shared library.
#             SHARED

             # Provides a relative path to your source file(s).
#            ${SOURCES})
