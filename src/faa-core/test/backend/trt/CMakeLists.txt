project(faa_cv_trt_test)

set(FAA-C-INCLUDE_DIRS
        ${OpenCV_INCLUDE_PATH}
        ${CMAKE_SOURCE_DIR}/include
        ${NLOHMANN_JSON_INCLUDE_PATH}
)
set(FAA-C-LIBRARIES
        ${OpenCV_LIB_PATH}
)
set(FAA-C-LINK_LIB_NAME
        opencv_world4120
        ws2_32
)

function(ocr_det_test)
    add_executable(faa_ocr_det_test faa_ocr_det_model_test.cpp)
    add_definitions(-DNOMINMAX)
    add_dependencies          (faa_ocr_det_test FaaCore)
    target_include_directories(faa_ocr_det_test PUBLIC ${FAA-C-INCLUDE_DIRS})
    target_link_directories   (faa_ocr_det_test PUBLIC ${FAA-C-LIBRARIES})
    target_link_libraries(
            faa_ocr_det_test PUBLIC
            ${FAA-C-LINK_LIB_NAME} FaaCore
)
endfunction()


ocr_det_test()

