project(FaaInputsMs)


set(FAA_INPUT_MS_SOURCES
        ./faa_inputs_ms_run.cpp

        ./service/faa_inputs_ms_loop.cpp
        ./service/faa_inputs_ms_oper.cpp
        ./service/faa_inputs_ms_test.cpp

)


add_executable(FaaInputsMs ${FAA_INPUT_MS_SOURCES})
target_include_directories(FaaInputsMs PUBLIC ${FAA-C-INCLUDE_DIRS})
target_link_directories   (FaaInputsMs PUBLIC ${FAA-C-LIBRARIES})
target_link_libraries(
        FaaInputsMs PUBLIC
        ${FAA-C-LINK_LIB_NAME} FaaCloud
)