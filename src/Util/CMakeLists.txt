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
       Register.cpp
       Register.h
       CoupSim_XML.cpp
       CoupSim_XML.h
       CoupModel_DB_Link.hpp
)

add_library(${LIBUTIL_NAME} STATIC ${LIBUTIL_SOURCES})

target_include_directories(${LIBUTIL_NAME} PUBLIC
    "./"
    "../"
    "${CMAKE_BINARY_DIR}/configured_files/include"
 )
 if(USE_POSTGRES) 
 target_link_libraries(${LIBUTIL_NAME} PUBLIC
    libpqxx::pqxx   
    cxxopts::cxxopts
    nlohmann_json::nlohmann_json
 )
 else()
  target_link_libraries(${LIBUTIL_NAME} PUBLIC
    nlohmann_json::nlohmann_json
 )
endif()