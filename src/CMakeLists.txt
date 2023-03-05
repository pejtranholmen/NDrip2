


add_subdirectory(Util)
add_subdirectory(PG)
add_subdirectory(Simulator)
add_subdirectory(SoilData)
add_subdirectory(ModelTypes)
add_subdirectory(NewBase)
add_subdirectory(Model)

add_executable(upload_xmlfile main.cpp )
add_executable(download_xmlfile mainRun.cpp)

if(USE_POSTGRES) 
		target_link_libraries(upload_xmlfile libpqxx::pqxx documentlib ${SIMULATOR_LIBNAME})
		target_link_libraries(download_xmlfile libpqxx::pqxx documentlib ${SIMULATOR_LIBNAME})
endif()






