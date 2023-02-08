set(EXE_SOURCES
    "main.cc"
)

add_executable(${EXECUTABLE_NAME} ${EXE_SOURCES})

target_include_directories(${EXECUTABLE_NAME}
    PRIVATE
    "../src"
    "${CMAKE_BINARY_DIR}/configured_files/include"
)


target_link_libraries(model INTERFACE pglib)
target_link_libraries(model INTERFACE modeltypeslib)


target_link_libraries(documentlib INTERFACE pglib )
target_link_libraries(documentlib INTERFACE model)
target_link_libraries(documentlib INTERFACE libutil)
target_link_libraries(documentlib INTERFACE libsoilpf)
target_link_libraries(documentlib INTERFACE mcmclib)

target_link_libraries(${EXECUTABLE_NAME} PUBLIC 
    documentlib
    ${SIMULATOR_LIBNAME}
 )
