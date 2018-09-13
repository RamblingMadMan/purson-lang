find_package(Boost 1.67.0 REQUIRED COMPONENTS filesystem)

set(
    PURSON_COMP_SOURCES
    main.cpp
)

add_executable(purson-comp ${PURSON_COMP_SOURCES})

target_link_libraries(purson-comp purson fmt Boost::filesystem)
