set(DOCUMENT_NAME documentlib)
add_library(${DOCUMENT_NAME})
target_sources(
	${DOCUMENT_NAME}
	PRIVATE
	    Bayes/BayesStatistics.cpp
        Bayes/BayesStatistics.h
        ParValueChangeRead/ChangeFromFile.cpp
        ParValueChangeRead/ChangeFromFile.h
        CH.cpp
        CH.h
        CommonModelInfo.cpp
        CommonModelInfo.h
        Doc.cpp
        Doc.h
        DB.cpp
        DB.h
        History.cpp
        History.h
        ModelBase.cpp
        ModelBase.h
        ModelFiles.cpp
        ModelFiles.h
        ModelMap.cpp
        ModelMap.h
        MR.cpp
        MR.h
        NewMap.cpp
        Sim.cpp
        Sim.h
        SimArchive.cpp
        SimArchive.h
)
add_subdirectory(MultiRun)
add_subdirectory(Validation)
add_subdirectory(DB)
add_subdirectory(mcmc)

#target_include_directories(
#	modeltypeslib
#	PRIVATE src/modeltypes)

target_include_directories(${DOCUMENT_NAME} PUBLIC
    "./"
    "../"
    "${CMAKE_BINARY_DIR}/configured_files/include"
)
if(USE_POSTGRES)
target_link_libraries(${DOCUMENT_NAME} 
PUBLIC 
if(MSVC)
    libpqxx::pqxx
endif()
    nlohmann_json::nlohmann_json 
)
else ()
target_link_libraries(${DOCUMENT_NAME} 
PUBLIC 
    nlohmann_json::nlohmann_json 
)



endif()
