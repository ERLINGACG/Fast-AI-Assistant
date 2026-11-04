cmake_minimum_required(VERSION 4.1)
project(FastAIAssistant-Root)

set(CMAKE_CXX_STANDARD 23)
if(MSVC)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /utf-8")
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} /utf-8")
endif()

set(DEBUG_INPUT_LIB E:/CPPlib/SelfLib/cppinterception/build/lib/Release)
set(DEBUG_INPUT_INCLUDE_PATH E:/CPPlib/SelfLib/cppinterception/include)


set(OpenCV_INCLUDE_PATH CACHE PATH "Path to OpenCV include directory")
set(OpenCV_LIB_PATH     CACHE PATH "Path to OpenCV library directory")

set(GDLZ_INCLUDE_PATH CACHE PATH "Path to GDLZ include directory")
set(GDLZ_TRT_LIB_PATH     CACHE PATH "Path to GDLZ TensorRT library directory")

set(DYLOG_INCLUDE_PATH CACHE PATH "Path to dylog include directory")
set(NLOHMANN_JSON_INCLUDE_PATH CACHE PATH "Path to nlohmann json include directory")

set(TensorRT_INCLUDE_PATH CACHE PATH "Path to TensorRT include directory")
set(CUDA_INCLUDE_PATH CACHE PATH "Path to CUDA include directory")

set(CPP_YAML_INCLUDE_PATH CACHE PATH "Path to cpp-yaml include directory")
set(CPP_YAML_LIB_PATH CACHE PATH "Path to cpp-yaml library directory")

#set(TensorRT_LIB_PATH CACHE PATH "Path to TensorRT library directory")


message("========OpenCV Config=========")
message("Opencv version: 4.12.0")
message("OpenCV_INCLUDE_PATH: ${OpenCV_INCLUDE_PATH}")
message("OpenCV_LIB_PATH: ${OpenCV_LIB_PATH}")
message("========OpenCV Config=========")

message("NLOHMANN_JSON_INCLUDE_PATH: ${NLOHMANN_JSON_INCLUDE_PATH}")
message("DYLOG_INCLUDE_PATH: ${DYLOG_INCLUDE_PATH}")


message("GDLZ_TRT_LIB_PATH: ${GDLZ_TRT_LIB_PATH}")
message("GDLZ_INCLUDE_PATH: ${GDLZ_INCLUDE_PATH}")

message("TensorRT_INCLUDE_PATH: ${TensorRT_INCLUDE_PATH}")
message("CUDA_INCLUDE_PATH: ${CUDA_INCLUDE_PATH}")

message("CPP_YAML_INCLUDE_PATH: ${CPP_YAML_INCLUDE_PATH}")
message("CPP_YAML_LIB_PATH: ${CPP_YAML_LIB_PATH}")


set(SRC_INCLUDE_DIRS
        ${OpenCV_INCLUDE_PATH}
        ./include
)
set(SRC_LIB_DIRS
        ${OpenCV_LIB_PATH}
        ${NLOHMANN_JSON_INCLUDE_PATH}
)
set(LINK_LIB_NAME
        opencv_world4120
)





add_subdirectory(src/faa-core)
add_subdirectory(src/faa-cloud)
