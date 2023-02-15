set(LIBUTIL_NAME libutil)

set(LIBUTIL_SOURCES
       Constants.cpp
       Constants.h
       StatUtil.cpp
       StatUtil.h
       ReadWriteTypes.cpp
       ReadWriteTypes.h 
       ModelUtil.cpp
       ModelUtil.h
       pugixml.cpp
       pugixml.hpp
       FUtil.hpp
       Register.hpp
       CoupSim_XML.cpp
       CoupSim_XML.h
       CoupModel_DB_Link.hpp
       FileSystemUtil.hpp
)

add_library(${LIBUTIL_NAME} STATIC ${LIBUTIL_SOURCES})

target_include_directories(${LIBUTIL_NAME} PUBLIC
    "./"
    "../"
    "${CMAKE_BINARY_DIR}/configured_files/include"
 )
 target_link_libraries(${LIBUTIL_NAME} PUBLIC
    libpqxx::pqxx   
    cxxopts::cxxopts
    nlohmann_json::nlohmann_json
 )