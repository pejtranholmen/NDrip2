add_executable(cpp_test test.cpp)
target_link_libraries(cpp_test modeltypes)
target_link_directories(cpp_test PRIVATE ${CURRENT_LIST_DIR})

