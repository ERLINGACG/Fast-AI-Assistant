project(FaaOcrRun)


add_executable(FaaOcrDetRun faa_ocr_det_run.cpp)
target_include_directories(FaaOcrDetRun PUBLIC  ${FAA-C-INCLUDE_DIRS} )
target_link_directories(FaaOcrDetRun PUBLIC ${FAA-C-LIBRARIES})
target_link_libraries(FaaOcrDetRun PUBLIC ${FAA-C-LINK_LIB_NAME})

add_executable(FaaOcrRecRun faa_ocr_res_build.cpp)
target_include_directories(FaaOcrRecRun PUBLIC  ${FAA-C-INCLUDE_DIRS} )
target_link_directories(FaaOcrRecRun PUBLIC ${FAA-C-LIBRARIES})
target_link_libraries(FaaOcrRecRun PUBLIC ${FAA-C-LINK_LIB_NAME}  yaml-cpp)
target_compile_definitions(FaaOcrRecRun PUBLIC YAML_CPP_STATIC_DEFINE)
