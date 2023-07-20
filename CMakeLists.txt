cmake_minimum_required(VERSION 3.14)
project(HyperLPR3-Source)

set(CMAKE_CXX_STANDARD 11)

set(LIBRARY_NAME hyperlpr3)

option( LINUX_FETCH_MNN             "Fetch and build MNN from git"  ON )
option( LINUX_USE_3RDPARTY_OPENCV   "Linux platform using pre-compiled OpenCV library from 3rdparty_hyper_inspire_op" OFF)
option( BUILD_SHARE                 "Build shared libs"                 ON )
option( BUILD_SAMPLES               "Build samples demo"                OFF )
option( BUILD_TEST                  "Build unit-test exec"              OFF )


set(PATH_3RDPARTY ${CMAKE_CURRENT_SOURCE_DIR}/3rdparty_hyper_inspire_op)

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

# must use mnn
add_definitions("-DINFERENCE_HELPER_ENABLE_MNN")

set(LINK_THIRD_LIBS pthread MNN m)

if (ANDROID)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 ")
    set(CMAKE_C_FLAGS "${CMAKE_CXX_FLAGS}")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")
    file(GLOB_RECURSE NATIVE_CPP_FILES ${CMAKE_CURRENT_SOURCE_DIR}/cpp/platform/jni/android/*.cpp)

    find_package(OpenCV REQUIRED)
    include_directories(${MNN_INCLUDE_DIRS})
    link_directories(${MNN_LIBS})

    add_library(${LIBRARY_NAME} SHARED ${NATIVE_CPP_FILES} ${SRC_C_CPP_FILES} ${CAPI_CC_FILES})
    target_link_libraries(${LIBRARY_NAME} ${OpenCV_LIBS} jnigraphics log -Wl,--whole-archive MNN -Wl,--no-whole-archive)

elseif(IOS)
    # TODO: Not implement
else ()
    # Build Linux or MacOS
    if (BUILD_CUDA)
        # TODO: Not implement
    elseif (BUILD_LINUX_ARM7)
        message("[BUILD]Linux Armv7")
        add_definitions("-DLINUX_ARM7")
        set(PLAT linux-arm32)
        # MNN Third party dependence
        set(MNN_INCLUDE_DIRS ${PATH_3RDPARTY}/MNN-2.2.0/${PLAT}-static/include)
        set(MNN_LIBS ${PATH_3RDPARTY}/MNN-2.2.0/${PLAT}-static/lib)
        # OpenCV Third party dependence
        set(OpenCV_DIR ${PATH_3RDPARTY}/opencv/opencv-linux-armhf/share/OpenCV)
        set(OpenCV_STATIC_INCLUDE_DIR ${PATH_3RDPARTY}/opencv/opencv-linux-armhf/include/)
        find_package(OpenCV REQUIRED)

    elseif (BUILD_LINUX_ARM64)
        # TODO: Not implement
    else()
        # Local Build
        message("[BUILD]Local")
        if (APPLE)
            set(PLAT darwin)
        else()
            set(PLAT linux)
        endif ()

        # Fetch MNN from git if the option is enabled
        if (LINUX_FETCH_MNN)
            # Include FetchContent module
            include(FetchContent)
            # Fetch MNN
            message("Downloading MNN from https://github.com/alibaba/MNN.git, this may take a while.")
            FetchContent_Declare(
                    mnn
                    GIT_REPOSITORY https://github.com/alibaba/MNN.git
                    GIT_TAG        2.2.0
            )
            set(MNN_BUILD_SHARED_LIBS OFF CACHE BOOL "" FORCE)
            set(MNN_BUILD_TOOLS OFF CACHE BOOL "" FORCE)

            FetchContent_MakeAvailable(mnn)
            set(MNN_INCLUDE_DIRS ${mnn_SOURCE_DIR}/include)
            set(MNN_LIBS ${mnn_BINARY_DIR})
        else()
            # MNN Third party dependence
            set(MNN_INCLUDE_DIRS ${PATH_3RDPARTY}/MNN-2.2.0/${PLAT}/include)
            set(MNN_LIBS ${PATH_3RDPARTY}/MNN-2.2.0/${PLAT}/lib)
        endif()


        if(LINUX_USE_3RDPARTY_OPENCV)
            # OpenCV Third party dependence （static）
            set(OpenCV_DIR ${PATH_3RDPARTY}/opencv-4.5.1/${PLAT}/lib/cmake/opencv4)
            set(OpenCV_STATIC_INCLUDE_DIR ${PATH_3RDPARTY}/opencv-4.5.1/${PLAT}/include/opencv4)
        endif()
        find_package(OpenCV REQUIRED)

    endif()
endif()

if (NOT ANDROID)
    # mnn
    message(MNN_INCLUDE_DIRS=${MNN_INCLUDE_DIRS})
    message(MNN_LIBS=${MNN_LIBS})
    include_directories(${MNN_INCLUDE_DIRS})
    link_directories(${MNN_LIBS})
    # opencv
    message(OpenCV_Version: ${OpenCV_VERSION})
    message(libraries: ${OpenCV_LIBS})
    message(libraries path: ${OpenCV_DIR})
    message(OpenCV_INCLUDE_DIRS=${OpenCV_STATIC_INCLUDE_DIR})
    include_directories(${OpenCV_STATIC_INCLUDE_DIR})

    if (BUILD_SAMPLES)
        # built samples exec
        add_executable(ContextSample ${CMAKE_CURRENT_SOURCE_DIR}/cpp/samples/sample_context.cpp ${SRC_C_CPP_FILES})
        target_link_libraries(ContextSample ${OpenCV_LIBS} ${LINK_THIRD_LIBS} )

        add_executable(SplitDetSample ${CMAKE_CURRENT_SOURCE_DIR}/cpp/samples/sample_split_model.cpp ${SRC_C_CPP_FILES})
        target_link_libraries(SplitDetSample ${OpenCV_LIBS} ${LINK_THIRD_LIBS} )

        add_executable(CAPISample ${CMAKE_CURRENT_SOURCE_DIR}/cpp/samples/sample_capi.cpp ${SRC_C_CPP_FILES} ${CAPI_CC_FILES})
        target_link_libraries(CAPISample ${OpenCV_LIBS} ${LINK_THIRD_LIBS} )

    endif()

    if (BUILD_TEST)
        if (ENABLE_BENCHMARK_TEST)
            message([Test]Open Benchmark Test)
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
message(CMAKE_INSTALL_PREFIX=${CMAKE_INSTALL_PREFIX})

# Add HyperLPR3-related files and directories to the installation list
install(TARGETS ${LIBRARY_NAME} DESTINATION ${CMAKE_INSTALL_PREFIX}/lib)
install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/cpp/c_api/hyper_lpr_sdk.h DESTINATION ${CMAKE_INSTALL_PREFIX}/include)

# Install resource file
install(DIRECTORY resource DESTINATION ${CMAKE_INSTALL_PREFIX})

# Install Resource files You can also install sample programs if they are available
if (BUILD_SAMPLES)
    install(TARGETS ContextSample DESTINATION ${CMAKE_INSTALL_PREFIX}/bin)
    install(TARGETS CAPISample DESTINATION ${CMAKE_INSTALL_PREFIX}/bin)
    install(TARGETS SplitDetSample DESTINATION ${CMAKE_INSTALL_PREFIX}/bin)
endif()
