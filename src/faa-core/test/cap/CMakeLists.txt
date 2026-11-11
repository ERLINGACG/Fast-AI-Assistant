project(faa_cap_test)



function(wgc_cap_test)
    add_executable(faa_wgc_cap_test wgc_show_test.cpp)
    add_definitions(-DNOMINMAX)
    add_dependencies          (faa_wgc_cap_test FaaCore)
    target_include_directories(faa_wgc_cap_test PUBLIC ${FAA-C-INCLUDE_DIRS})
    target_link_libraries(
            faa_wgc_cap_test PUBLIC
             FaaCore
    )
endfunction()

function(stream_cap_test_pull)
    add_executable(faa_stream_cap_test_pull stream_cap_test_pull.cpp)
    add_definitions(-DNOMINMAX)
    add_dependencies          (faa_stream_cap_test_pull FaaCore)
    target_include_directories(faa_stream_cap_test_pull PUBLIC ${FAA-C-INCLUDE_DIRS})
    target_link_libraries(
            faa_stream_cap_test_pull PUBLIC
            FaaCore
    )
endfunction()


wgc_cap_test()
stream_cap_test_pull()

