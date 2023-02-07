add_library(conversion INTERFACE)

target_sources(conversion INTERFACE
    FILE_SET HEADERS
    BASE_DIRS ${CMAKE_CURRENT_LIST_DIR}
    FILES is_arithmetric.h
 )


