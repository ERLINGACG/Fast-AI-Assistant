project(logger_test)


add_executable(faa_logger_test faa_logger_test.cpp)
target_include_directories(faa_logger_test PUBLIC
        ${CMAKE_SOURCE_DIR}/include
        ${NLOHMANN_JSON_INCLUDE_PATH}
)
target_link_libraries(faa_logger_test FaaCore)
