cmake_minimum_required(VERSION 3.10.0 FATAL_ERROR)

project(purson)

set(CMAKE_CXX_STANDARD 17)

include_directories("${PROJECT_SOURCE_DIR}/include")
include_directories("${PROJECT_SOURCE_DIR}/utfcpp/source")

add_subdirectory(fmt)
add_subdirectory(lib)
add_subdirectory(repl)
