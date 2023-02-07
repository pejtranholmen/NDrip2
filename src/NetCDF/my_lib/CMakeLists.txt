set(LIBRARY_SOURCES
    "my_lib.cc"
   "ModelConst.cpp"
    "../Util/StatUtil.cpp"
)
set(LIBRARY_HEADERS
    "my_lib.h"
    "ModelConst.h"
    "../Util/StatUtil.h"
 )
set(LIBRARY_INCLUDES
    "./"
    "../"
    "${CMAKE_BINARY_DIR}/configured_files/include")

add_library(${LIBRARY_NAME} STATIC
    ${LIBRARY_SOURCES}
    ${LIBRARY_HEADERS})
target_include_directories(${LIBRARY_NAME} PUBLIC
    ${LIBRARY_INCLUDES})

