cmake_minimum_required(VERSION 3.14)
project(HyperLPR3-Source)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} --std=c++14")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS}")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O3")

set(HYPERLPR_VERSION_MAJOR 3)
set(HYPERLPR_VERSION_MINOR 1)
set(HYPERLPR_VERSION_PATCH 0)
# Converts the version number to a string
string(CONCAT HYPERLPR_VERSION_MAJOR_STR ${HYPERLPR_VERSION_MAJOR})
string(CONCAT HYPERLPR_VERSION_MINOR_STR ${HYPERLPR_VERSION_MINOR})
string(CONCAT HYPERLPR_VERSION_PATCH_STR ${HYPERLPR_VERSION_PATCH})
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/cpp/src/version.h.in ${CMAKE_CURRENT_SOURCE_DIR}/cpp/src/version.h)

set(LIBRARY_NAME hyperlpr3)

option( LINUX_USE_3RDPARTY_OPENCV   "Linux platform using pre-compiled OpenCV library from 3rdparty_hyper_inspire_op" OFF)
option( BUILD_SHARE                 "Build shared libs"                 ON )
option( BUILD_SAMPLES               "Build samples demo"                ON )
option( BUILD_TEST                  "Build unit-test exec"              ON )


set(PATH_3RDPARTY ${CMAKE_CURRENT_SOURCE_DIR}/3rdparty_hyper_inspire_op)
set(HYPERLPR_3RDPARTY ${CMAKE_CURRENT_SOURCE_DIR}/3rdparty)

# find all cpp file
file(GLOB_RECURSE SRC_BUFFER_MODULE_FILES ${CMAKE_CURRENT_SOURCE_DIR}/cpp/src/buffer_module/*.cpp)
file(GLOB_RECURSE SRC_CONTEXT_MODULE_FILES ${CMAKE_CURRENT_SOURCE_DIR}/cpp/src/context_module/*.cpp)
file(GLOB_RECURSE SRC_NN_MODULE_FILES ${CMAKE_CURRENT_SOURCE_DIR}/cpp/src/nn_module/*.cpp)
file(GLOB_RECURSE SRC_NN_IMPL_MODULE_FILES ${CMAKE_CURRENT_SOURCE_DIR}/cpp/src/nn_implementation_module/*.cpp)
file(GLOB SRC_FILES ${CMAKE_CURRENT_SOURCE_DIR}/cpp/src/*.cpp)
# local files
set(SRC_INFERENCE_HELPER_LOCAL_FILES ${CMAKE_CURRENT_SOURCE_DIR}/cpp/src/inference_helper_module/inference_helper.cpp cpp/src/inference_helper_module/inference_helper_mnn.cpp)
# include src header
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/cpp/src)


set(SRC_C_CPP_FILES ${SRC_BUFFER_MODULE_FILES}
        ${SRC_CONTEXT_MODULE_FILES}
        ${SRC_LOADER_MODULE_FILES}
        ${SRC_NN_MODULE_FILES}
        ${SRC_NN_IMPL_MODULE_FILES}
        ${SRC_SLOG_MODULE_FILES}
        ${SRC_FILES}
        ${SRC_INFERENCE_HELPER_LOCAL_FILES})

# find all c file for c_api
file(GLOB_RECURSE CAPI_CC_FILES ${CMAKE_CURRENT_SOURCE_DIR}/cpp/c_api/*.cc)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/cpp/c_api)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/cpp/platform)


add_definitions("-DINFERENCE_HELPER_ENABLE_MNN")

# must use mnn
if(DEFINED MNN_STATIC_PATH) 
    set(MNN_INCLUDE_DIRS "${MNN_STATIC_PATH}/include")
    set(MNN_LIBS "${MNN_STATIC_PATH}/lib/libMNN.a")
else ()
    # MNN Options
    set(MNN_BUILD_SHARED_LIBS ON CACHE BOOL "Build MNN as a shared library")
    set(MNN_USE_THREAD_POOL OFF)
    add_subdirectory(${HYPERLPR_3RDPARTY}/MNN EXCLUDE_FROM_ALL)
    set(MNN_INCLUDE_DIRS "${HYPERLPR_3RDPARTY}/MNN/include")
    set(MNN_LIBS MNN)
endif()

include_directories(${MNN_INCLUDE_DIRS})


set(LINK_THIRD_LIBS pthread ${MNN_LIBS} m)

if (ANDROID)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 ")
    set(CMAKE_C_FLAGS "${CMAKE_CXX_FLAGS}")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")
    file(GLOB_RECURSE NATIVE_CPP_FILES ${CMAKE_CURRENT_SOURCE_DIR}/cpp/platform/jni/android/*.cpp)

    add_library(${LIBRARY_NAME} SHARED ${NATIVE_CPP_FILES} ${SRC_C_CPP_FILES} ${CAPI_CC_FILES})
    target_link_libraries(${LIBRARY_NAME} ${OpenCV_LIBS} jnigraphics log -Wl,--whole-archive MNN -Wl,--no-whole-archive)

elseif(IOS)
    # TODO: Not implement
else ()
    # Build Linux or MacOS
    if (BUILD_CUDA)
        # TODO: Not implement
    elseif (BUILD_LINUX_ARM7)
        add_definitions("-DLINUX_ARM7")
        set(PLAT linux-arm32)
        # OpenCV Third party dependence
        set(OpenCV_DIR ${PATH_3RDPARTY}/opencv/opencv-linux-armhf/share/OpenCV)
        set(OpenCV_STATIC_INCLUDE_DIR ${PATH_3RDPARTY}/opencv/opencv-linux-armhf/include/)

    elseif (BUILD_LINUX_ARM_AARCH64)
        add_definitions("-DLINUX_ARM_AARCH64")
        # OpenCV Third party dependence
        set(OpenCV_DIR ${PATH_3RDPARTY}/opencv-linux-aarch64-4.5.1-shared/lib/cmake/opencv4)
        set(OpenCV_STATIC_INCLUDE_DIR ${PATH_3RDPARTY}/opencv-linux-aarch64-4.5.1-shared/include/opencv4)
    else()
        # Local Build
        if (APPLE)
            set(PLAT darwin)
        else()
            set(PLAT linux)
        endif ()

        if(LINUX_USE_3RDPARTY_OPENCV)
            # OpenCV Third party dependence （static）
            set(OpenCV_DIR ${PATH_3RDPARTY}/opencv-4.5.1/${PLAT}/lib/cmake/opencv4)
            set(OpenCV_STATIC_INCLUDE_DIR ${PATH_3RDPARTY}/opencv-4.5.1/${PLAT}/include/opencv4)
        endif()

    endif()
endif()

find_package(OpenCV REQUIRED)

if (NOT ANDROID)
    # opencv
    include_directories(${OpenCV_STATIC_INCLUDE_DIR})

    if (BUILD_SAMPLES)
        # built samples exec
        add_executable(ContextSample ${CMAKE_CURRENT_SOURCE_DIR}/cpp/samples/sample_context.cpp ${SRC_C_CPP_FILES})
        target_link_libraries(ContextSample ${OpenCV_LIBS} ${LINK_THIRD_LIBS} )

#        add_executable(SplitDetSample ${CMAKE_CURRENT_SOURCE_DIR}/cpp/samples/sample_split_model.cpp ${SRC_C_CPP_FILES})
#        target_link_libraries(SplitDetSample ${OpenCV_LIBS} ${LINK_THIRD_LIBS} )

        add_executable(CAPISample ${CMAKE_CURRENT_SOURCE_DIR}/cpp/samples/sample_capi.cpp ${SRC_C_CPP_FILES} ${CAPI_CC_FILES})
        target_link_libraries(CAPISample ${OpenCV_LIBS} ${LINK_THIRD_LIBS} )

#        add_executable(CAPIDetectSample ${CMAKE_CURRENT_SOURCE_DIR}/cpp/samples/sample_capi_detect.cpp ${SRC_C_CPP_FILES} ${CAPI_CC_FILES})
#        target_link_libraries(CAPIDetectSample ${OpenCV_LIBS} ${LINK_THIRD_LIBS} )

    endif()

    if (BUILD_TEST)
        if (ENABLE_BENCHMARK_TEST)
            add_definitions(-DENABLE_BENCHMARK_TEST)
        endif ()
        file(GLOB_RECURSE TEST_C_CPP_FILES ${CMAKE_CURRENT_SOURCE_DIR}/cpp/test/*.cpp)
        add_executable(UnitTest ${TEST_C_CPP_FILES} ${SRC_C_CPP_FILES} ${CAPI_CC_FILES})
        target_link_libraries(UnitTest ${OpenCV_LIBS} ${LINK_THIRD_LIBS})
    endif()

    if (BUILD_SHARE)
        # build dynamic library
        add_library(${LIBRARY_NAME} SHARED ${SRC_C_CPP_FILES} ${CAPI_CC_FILES})
        target_link_libraries(${LIBRARY_NAME} ${OpenCV_LIBS} ${LINK_THIRD_LIBS} )
        set(SRC_RKNN_RELATED ${CMAKE_CURRENT_SOURCE_DIR}/cpp/src/inference_helper_rknn.cpp)
        set(SRC_C_CPP_FILES ${SRC_C_CPP_FILES} )

    endif()

endif()


set(CMAKE_INSTALL_PREFIX ${PROJECT_BINARY_DIR}/install/hyperlpr3)

message(STATUS ">>>>>>>>>>>>>")
message(STATUS "HyperLPR BUILD INFO:")
message(STATUS "\tSystem: ${CMAKE_SYSTEM_NAME}")
message(STATUS "\tProcessor: ${CMAKE_SYSTEM_PROCESSOR}")
message(STATUS "\tVersion: ${HYPERLPR_VERSION_MAJOR}.${HYPERLPR_VERSION_MINOR}.${HYPERLPR_VERSION_PATCH}")
message(STATUS "\tOpenCV: ${OpenCV_DIR}")
message(STATUS "\tMNN: ${MNN_LIBS}")
message(STATUS "\tBuild Share: ${BUILD_SHARE}")
message(STATUS "\tBuild Sample: ${BUILD_SAMPLES}")
message(STATUS "\tBuild Test: ${BUILD_TEST}")
message(STATUS "\tInstall To: ${CMAKE_INSTALL_PREFIX}")


# Add HyperLPR3-related files and directories to the installation list
install(TARGETS ${LIBRARY_NAME} DESTINATION ${CMAKE_INSTALL_PREFIX}/lib)
install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/cpp/c_api/hyper_lpr_sdk.h DESTINATION ${CMAKE_INSTALL_PREFIX}/include)

# Install resource file
install(DIRECTORY resource DESTINATION ${CMAKE_INSTALL_PREFIX})

# Install Resource files You can also install sample programs if they are available
if (BUILD_SAMPLES)
#    install(TARGETS ContextSample DESTINATION ${CMAKE_INSTALL_PREFIX}/bin)
    install(TARGETS CAPISample DESTINATION ${CMAKE_INSTALL_PREFIX}/bin)
#    install(TARGETS SplitDetSample DESTINATION ${CMAKE_INSTALL_PREFIX}/bin)
endif()