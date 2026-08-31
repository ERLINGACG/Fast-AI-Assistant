project(FaaCore)
set(FAA-C-SOURCES
       win_utils/windows_utils.cpp
       input/windows_input.cpp
       wgc_cap/faa_wgc_cap.cpp

       fdnn/fcv/faa_dnn_cv_trt.cpp
       fdnn/fcv/processer/ocr/faa_ocr_det_proc.cpp
       fdnn/fcv/processer/ocr/faa_ocr_res_proc.cpp
)
set(FAA-C-INCLUDE_DIRS
        ${CMAKE_SOURCE_DIR}/include
        ${NLOHMANN_JSON_INCLUDE_PATH}
        ${OpenCV_INCLUDE_PATH}
        ${GDLZ_INCLUDE_PATH}
        ${DYLOG_INCLUDE_PATH}
        ${TensorRT_INCLUDE_PATH}
        ${CUDA_INCLUDE_PATH}
        ${CPP_YAML_INCLUDE_PATH}
        ${DEBUG_INPUT_INCLUDE_PATH}
)
set(FAA-C-LIBRARIES
        ${OpenCV_LIB_PATH}
        ${GDLZ_TRT_LIB_PATH}
        ${CPP_YAML_LIB_PATH}
        ${DEBUG_INPUT_LIB}
)
set(FAA-C-LINK_LIB_NAME
        opencv_world4120
        ws2_32
        d3d11
        GeneralDnnLib_Zero_trt_core
        GeneralDnnLib_Zero_trt_cv
        interception.lib
)

add_library(FaaCore SHARED ${FAA-C-SOURCES})
target_include_directories(FaaCore PUBLIC ${FAA-C-INCLUDE_DIRS})
target_link_directories(FaaCore PUBLIC ${FAA-C-LIBRARIES})
target_link_libraries(FaaCore PUBLIC ${FAA-C-LINK_LIB_NAME})


add_subdirectory(faa_ocr/run)

function(input_test)
    add_executable(input_test test/trt_cv_test.cpp)
    target_link_libraries(input_test PUBLIC FaaCore)
endfunction()

function(get_frame_run)
    add_executable(FaaGetFrameRun utils/get_frame_run.cpp)
    target_link_libraries(FaaGetFrameRun PUBLIC FaaCore)
endfunction()

function(build_trt_engine)
    add_executable(FaaBuildTrtEngine utils/build_trt_engine.cpp)
    target_link_libraries(FaaBuildTrtEngine PUBLIC FaaCore)
endfunction()

input_test()
get_frame_run()
build_trt_engine()
