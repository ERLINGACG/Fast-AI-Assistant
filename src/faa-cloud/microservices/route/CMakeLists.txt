project(FaaRouteMs)


set(FAA_ROUTE_MS_SOURCES
    ./faa_route_ms_run.cpp

   ./service/faa_route_ms_data_imp.cpp
   ./service/faa_route_ms_loop.cpp
   ./service/faa_route_ms_oper.cpp
)


add_executable(FaaRouteMs ${FAA_ROUTE_MS_SOURCES})
target_include_directories(FaaRouteMs PUBLIC ${FAA-C-INCLUDE_DIRS})
target_link_directories   (FaaRouteMs PUBLIC ${FAA-C-LIBRARIES})
target_link_libraries(
        FaaRouteMs PUBLIC
        ${FAA-C-LINK_LIB_NAME} FaaCloud
)