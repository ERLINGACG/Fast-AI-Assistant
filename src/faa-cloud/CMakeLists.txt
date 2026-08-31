
project(FaaCloud)





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


function(build_cloud_lib)

    set(FAA-C-SOURCES
            network/faa_cloud_network.cpp
            common/frame/faa_cloud_wgc_loop.cpp
    )
    add_library(FaaCloud SHARED ${FAA-C-SOURCES})
    add_definitions(-DNOMINMAX)
    add_dependencies(FaaCloud FaaCore)
    target_include_directories(FaaCloud PUBLIC ${FAA-C-INCLUDE_DIRS})
    target_link_directories(FaaCloud PUBLIC ${FAA-C-LIBRARIES})
    target_link_libraries(
        FaaCloud PUBLIC
        ${FAA-C-LINK_LIB_NAME} FaaCore
    )
endfunction()

function(build_cloud_router_exec)
    set(FAA-C-Router-SOURCES
            router/faa_cloud_router.cpp
            run/faa_cloud_router_run.cpp
    )
    add_definitions(-DNOMINMAX)
    add_executable(FaaCloudRouter ${FAA-C-Router-SOURCES})
    add_dependencies(FaaCloudRouter FaaCloud)
    target_include_directories(FaaCloudRouter PUBLIC ${FAA-C-INCLUDE_DIRS})
    target_link_directories(FaaCloudRouter PUBLIC ${FAA-C-LIBRARIES})
    target_link_libraries(
            FaaCloudRouter PUBLIC
            ${FAA-C-LINK_LIB_NAME} FaaCloud
    )
endfunction()


function(build_cloud_ms_sub)
    add_subdirectory(microservices/arknights)
endfunction()

build_cloud_lib()
build_cloud_router_exec()
build_cloud_ms_sub()



