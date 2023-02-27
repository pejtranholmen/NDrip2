


add_subdirectory(Util)
add_subdirectory(PG)
add_subdirectory(Simulator)
add_subdirectory(SoilData)
add_subdirectory(ModelTypes)
add_subdirectory(NewBase)
add_subdirectory(Model)

add_executable(upload_xmlfile main.cpp)

if(USE_POSTGRES) 
		target_link_libraries(upload_xmlfile libpqxx::pqxx documentlib ${SIMULATOR_LIBNAME})
endif()






