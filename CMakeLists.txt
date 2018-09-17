cmake_minimum_required(VERSION 3.9.0 FATAL_ERROR)

project(purson)

option(BUILD_BEAR "Build Bear IDE" ON)

set(CPACK_RESOURCE_FILE_LICENSE "${PROJECT_SOURCE_DIR}/LICENCE")
set(CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/cmake")
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(ExternalProject)
include(CPack)

set(DEPENDENCY_DIR ${PROJECT_SOURCE_DIR}/deps)

include_directories(${DEPENDENCY_DIR}/fmt/include)
include_directories(${DEPENDENCY_DIR}/utfcpp/source)
include_directories(${PROJECT_SOURCE_DIR}/include)

add_subdirectory(${DEPENDENCY_DIR}/fmt)
add_subdirectory(lib)
add_subdirectory(repl)
add_subdirectory(comp)

if(BUILD_BEAR)
  add_subdirectory(bear)
endif()
