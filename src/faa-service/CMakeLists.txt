
project(FastAiAssistantServer)


set(FAA-S-SOURCES
       fast_ai_assistant_service.cpp
       fast_ai_assistant_service_run.cpp
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

add_executable(FastAiAssistantServer ${FAA-S-SOURCES})
add_dependencies(FastAiAssistantServer FaaCore)

target_include_directories(FastAiAssistantServer PUBLIC ${FAA-S-INCLUDE_DIRS})
target_link_directories(FastAiAssistantServer PUBLIC ${FAA-S-LIBRARIES})
target_link_libraries(
        FastAiAssistantServer PUBLIC
        ${FAA-S-LINK_LIB_NAME} FaaCore
)
