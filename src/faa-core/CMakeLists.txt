project(FaaCore)

set(FAA_INPUT_SRC
        input/keyboard/faa_keyboard_active.cpp
        input/keyboard/faa_keyboard_input.cpp
        input/keyboard/faa_keyboard_input_apply.cpp
        input/keyboard/faa_keyboard_params_build.cpp

        input/mouse/faa_mouse_input_move.cpp
        input/mouse/faa_mouse_input_click.cpp
        input/mouse/faa_mouse_input.cpp
        input/mouse/faa_mouse_params_build.cpp
        input/mouse/faa_mouse_input_drag.cpp
)

set(FAA_WIN_SRC
        win_utils/windows_utils.cpp
        input/windows_input.cpp
)

set(FAA_MAT_IMPI_SRC
        frame/private/faa_mat_impi.cpp
)

set(FAA_WGC_CAP_SRC
        cap/wgc_cap/private/faa_texture_impi.cpp
        cap/wgc_cap/private/faa_wgc_cap_impi.cpp
        cap/wgc_cap/faa_wgc_cap.cpp
        cap/wgc_cap/faa_wgc_cap_init.cpp
        cap/wgc_cap/faa_wgc_cap_oper.cpp
)

set(FAA_STREAM_CAP_SRC
        cap/stream_cap/faa_stream_cap.cpp
        cap/stream_cap/faa_stream_cap_pusher.cpp
        cap/stream_cap/faa_stream_cap_puller.cpp
        cap/stream_cap/private/faa_stream_cap_impl.cpp
        cap/stream_cap/faa_stream_cap_utils.cpp
)

set(FAA_DNN_TRT_BACKEND_SRC
        fdnn/backend/trt_cv/faa_cv_dnn_trt.cpp
        fdnn/backend/trt_cv/faa_cv_dnn_trt_init.cpp
        fdnn/backend/trt_cv/faa_cv_dnn_trt_run.cpp
        fdnn/backend/trt_cv/private/faa_cv_trt_framework_impl.cpp
        fdnn/backend/trt_cv/private/faa_cv_trt_ctx_impl.cpp
)

set(FAA_DNN_GENERAL_SRC
        fdnn/general/ocr/faa_cv_ocr_rec_pros.cpp
        fdnn/general/ocr/faa_cv_ocr_det_pros.cpp
        fdnn/general/blob/faa_cv_set_blob.cpp
        fdnn/general/mixformer_v2/faa_cv_mixformer_v2_pros.cpp
)

set(FAA_DNN_MODEL_SRC

        fdnn/model/base/faa_dnn_model_base.cpp
        fdnn/model/base/faa_dnn_layers.cpp
        fdnn/model/ocr/faa_ocr_det_model.cpp
        fdnn/model/ocr/faa_ocr_rec_model.cpp
        fdnn/model/mixformer_v2/faa_mixformer_v2_model.cpp
)

set(FAA_OLD_DNN_SRC
        fdnn/fcv/faa_dnn_cv_trt.cpp
        fdnn/fcv/processer/ocr/faa_ocr_det_proc.cpp
        fdnn/fcv/processer/ocr/faa_ocr_res_proc.cpp
        fdnn/fcv/processer/ob/faa_yolo_proc.cpp

        feature/faa_cv_surf_feature.cpp
        feature/faa_cv_hough_circle.cpp
        resource/faa_cv_resources.cpp
)

set(FAA-C-SOURCES
        ${FAA_WIN_SRC}
        ${FAA_OLD_DNN_SRC}
        ${FAA_DNN_GENERAL_SRC}
        ${FAA_DNN_MODEL_SRC}
        ${FAA_DNN_TRT_BACKEND_SRC}
        ${FAA_INPUT_SRC}
        ${FAA_MAT_IMPI_SRC}
        ${FAA_STREAM_CAP_SRC}
        ${FAA_WGC_CAP_SRC}
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
        ${FFMPEG_INCLUDE_PATH}
)
set(FAA-C-LIBRARIES
        ${OpenCV_LIB_PATH}
        ${GDLZ_TRT_LIB_PATH}
        ${CPP_YAML_LIB_PATH}
        ${DEBUG_INPUT_LIB}
        ${FFMPEG_LIB_PATH}
)
set(FAA-C-LINK_LIB_NAME
        opencv_world4120
        ws2_32
        d3d11
        GeneralDnnLib_Zero_trt_core
        GeneralDnnLib_Zero_trt_cv
        interception.lib
        avformat
        avcodec
        avutil
        swscale
)

add_library(FaaCore SHARED ${FAA-C-SOURCES})
target_include_directories(FaaCore PUBLIC ${FAA-C-INCLUDE_DIRS})
target_link_directories(FaaCore PUBLIC ${FAA-C-LIBRARIES})
target_link_libraries  (FaaCore PUBLIC ${FAA-C-LINK_LIB_NAME})


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

function(test_run)
  add_subdirectory(test/backend/trt)
  add_subdirectory(test/cap)
  add_subdirectory(test/input)
endfunction()

input_test()
get_frame_run()
build_trt_engine()
test_run()
