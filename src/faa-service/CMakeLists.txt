
project(FastAiAssistantServer)


set(FAA-S-SOURCES

#       service_runtime/faa_service_runtime.cpp
#       service_runtime/faa_service_runtime_init.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/service_network/faa_service_udp_network.cpp
#       faa_service_orchestrator.cpp
#       faa_service_operation.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/service_resources/faa_service_resources_ocr_init.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/service_event/faa_service_event.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/service_event/operation/input/faa_service_win32_operation.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/service_event/operation/ocr/faa_service_ocr_operation.cpp
)

set(FAA-S-INCLUDE_DIRS
        ${OpenCV_INCLUDE_PATH}
        ${CMAKE_SOURCE_DIR}/include
        ${NLOHMANN_JSON_INCLUDE_PATH}
)
set(FAA-S-LIBRARIES
        ${OpenCV_LIB_PATH}
)
set(FAA-S-LINK_LIB_NAME
        opencv_world4120
        ws2_32
)
add_definitions(-DNOMINMAX)
add_executable(FastAiAssistantServer ${FAA-S-SOURCES} fast_ai_assistant_service_run.cpp)
add_dependencies(FastAiAssistantServer FaaCore)

target_include_directories(FastAiAssistantServer PUBLIC ${FAA-S-INCLUDE_DIRS})
target_link_directories(FastAiAssistantServer PUBLIC ${FAA-S-LIBRARIES})
target_link_libraries(
        FastAiAssistantServer PUBLIC
        ${FAA-S-LINK_LIB_NAME} FaaCore
)

