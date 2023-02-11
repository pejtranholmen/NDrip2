


add_subdirectory(Util)
add_subdirectory(PG)
add_subdirectory(Simulator)
add_subdirectory(SoilData)
add_subdirectory(ModelTypes)
add_subdirectory(NewBase)
add_subdirectory(Model)



if(USE_POSTGRES) 
		add_executable(main main.cpp)
		target_link_libraries(main libpqxx::pqxx documentlib ${SIMULATOR_LIBNAME})
endif()






