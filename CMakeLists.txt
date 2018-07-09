cmake_minimum_required(VERSION 3.9.0 FATAL_ERROR)

project(purson)

set(CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/cmake")
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include_directories("${PROJECT_SOURCE_DIR}/include")
include_directories("${PROJECT_SOURCE_DIR}/utfcpp/source")

add_subdirectory(fmt)
add_subdirectory(lib)
add_subdirectory(repl)
