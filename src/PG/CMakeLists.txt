set(PG_LIB_NAME pglib)
set(PG_LIB_SOURCE  
        NoaaCoord.cpp
        NoaaCoord.h
        PG.CPP
        PG.H
        PG_MEMORY.cpp
        PG_MEMORY.h
        PG_Stream.cpp
        PG_Stream.h
        PGBase.cpp
        PGBase.h
        PGFile.cpp
        PGFile.h
        PGUtil.cpp
        PGUtil.h
        PGView.cpp
        PGView.h
        FileImport/PGFileImport.cpp
        FileImport/PGFileImport.h
        FileImport/PGFileImportBase.cpp
        FileImport/PGFileImportBase.h
        FileImport/ExtraLinkedInformation.cpp
        FileImport/ExtraLinkedInformation.h

)
add_library(${PG_LIB_NAME} STATIC ${PG_LIB_SOURCE}
)
target_include_directories(${PG_LIB_NAME} PUBLIC
    "./")