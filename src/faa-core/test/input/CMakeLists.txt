project(faa_keyboard_input_test)


function(add_faa_keyboard_input_test)
    add_executable(faa_keyboard_input_test faa_keyboard_test.cpp)
    target_include_directories(faa_keyboard_input_test PUBLIC ${FAA-C-INCLUDE_DIRS})
    target_link_libraries(
            faa_keyboard_input_test PUBLIC
            FaaCore
    )
endfunction()

function(add_faa_mouse_input_test)
    add_executable(faa_mouse_input_test faa_mouse_test.cpp)
    target_include_directories(faa_mouse_input_test PUBLIC ${FAA-C-INCLUDE_DIRS})
    target_link_libraries(
            faa_mouse_input_test PUBLIC
            FaaCore
    )
endfunction()



add_faa_keyboard_input_test()
add_faa_mouse_input_test()
