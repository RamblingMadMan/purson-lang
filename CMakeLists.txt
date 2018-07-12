cmake_minimum_required(VERSION 3.9.0 FATAL_ERROR)

project(purson)

include(ExternalProject)

set(CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/cmake")
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(DEPENDENCY_DIR ${PROJECT_SOURCE_DIR}/deps)

include_directories(${DEPENDENCY_DIR}/fmt-5.0.0/include)
include_directories(${DEPENDENCY_DIR}/utfcpp-2.3.5/source)
include_directories(${PROJECT_SOURCE_DIR}/include)

add_subdirectory(${DEPENDENCY_DIR}/fmt-5.0.0)
add_subdirectory(lib)
add_subdirectory(repl)
