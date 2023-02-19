cmake_minimum_required(VERSION 3.16)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(FetchContent)

project(CoupModelBase VERSION 1.0.0 LANGUAGES CXX)

set(SIMULATOR_LIBNAME SimLibrary)
set(EXECUTABLE_NAME Executable)

set(CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/cmake/")
include(AddGitSubmodule)
option(USE_POSTGRES "To include the link to a local Postgres Database" ON)


if(USE_POSTGRES) 

    find_package(libpqxx CONFIG REQUIRED)
    message("libpq will be used")
    add_compile_definitions(COUP_POSTGRES )

endif()



FetchContent_Declare(
    cxxopts
    GIT_REPOSITORY https://github.com/jarro2783/cxxopts
    GIT_TAG v3.0.0
    GIT_SHALLOW TRUE)

FetchContent_MakeAvailable(cxxopts)

FetchContent_Declare(json URL https://github.com/nlohmann/json/releases/download/v3.11.2/json.tar.xz)

FetchContent_MakeAvailable(json)



include(GNUInstallDirs)

if (MSVC)
    set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS ON)
    add_compile_definitions(MS_CODE)
else ()
    include(GNUInstallDirs)
endif()

add_compile_definitions(NO_MS_GUI COUPSTD STD)

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/${CMAKE_INSTALL_LIBDIR})
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/${CMAKE_INSTALL_LIBDIR})
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/${CMAKE_INSTALL_BINDIR})

# defines targets and sources
add_subdirectory(src)

# contains an "external" library we will link to
add_subdirectory(external)

add_subdirectory(configured)

add_subdirectory(app)

target_include_directories(Executable PRIVATE 
    "./"
    "${PROJECT_SOURCE_DIR}"
    "${CMAKE_BINARY_DIR}/configured_files/include"
)





# enable testing and define tests
#enable_testing()
#add_subdirectory(tests)
