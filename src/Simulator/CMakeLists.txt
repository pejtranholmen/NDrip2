set(LIBRARY_SOURCES
   "ModelConst.cpp"
    "ModelInfo.cpp"
    "DefineModel.cpp"
    "Simulator.cpp"
)
set(LIBRARY_HEADERS
    "ModelConst.h"
    "ModelInfo.h"
    "DefineModel.h"
    "Simulator.h"
)
set(LIBRARY_INCLUDES
    "./"
    "../"
    "${CMAKE_BINARY_DIR}/configured_files/include")

add_library(${SIMULATOR_LIBNAME} 
    ${LIBRARY_SOURCES}
    ${LIBRARY_HEADERS})
target_include_directories(${SIMULATOR_LIBNAME} PUBLIC
    ${LIBRARY_INCLUDES})


