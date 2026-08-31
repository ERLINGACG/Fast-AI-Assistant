project(FaaArknightsMs)


set(FAA_ARKNIGHTS_MS_SOURCES
        faa_arknights_run.cpp
        service/faa_cloud_arknights_service.cpp
        service/faa_cloud_arknights_operator.cpp
)


add_executable(FaaArknightsMs ${FAA_ARKNIGHTS_MS_SOURCES})
target_include_directories(FaaArknightsMs PUBLIC ${FAA-C-INCLUDE_DIRS})
target_link_directories(FaaArknightsMs PUBLIC ${FAA-C-LIBRARIES})
target_link_libraries(
        FaaArknightsMs PUBLIC
        ${FAA-C-LINK_LIB_NAME} FaaCloud
)