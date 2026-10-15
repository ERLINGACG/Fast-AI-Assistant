project(FaaGeneralMs)


set(FAA_GENERAL_MS_SOURCES
       faa_general_run.cpp
       service/faa_general_service_cmd.cpp
       service/faa_general_service_cap.cpp
       service/faa_general_network.cpp
       service/faa_general_service_operation.cpp
       service/ocr/faa_general_service_ocr_gpu.cpp
       service/ocr/faa_general_service_ocr_oper.cpp
       service/yolo/faa_general_service_yolo_gpu.cpp
       service/yolo/faa_general_service_yolo_oper.cpp
       service/input/faa_general_service_input_oper.cpp
)


add_executable(FaaGeneralMs ${FAA_GENERAL_MS_SOURCES})
target_include_directories(FaaGeneralMs PUBLIC ${FAA-C-INCLUDE_DIRS})
target_link_directories(FaaGeneralMs PUBLIC ${FAA-C-LIBRARIES})
target_link_libraries(
        FaaGeneralMs PUBLIC
        ${FAA-C-LINK_LIB_NAME} FaaCloud
)