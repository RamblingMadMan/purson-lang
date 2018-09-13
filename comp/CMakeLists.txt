set(
    PURSON_COMP_SOURCES
    main.cpp
)

add_executable(purson-comp ${PURSON_COMP_SOURCES})

target_link_libraries(purson-comp stdc++fs purson fmt)

install(
	TARGETS purson-comp
	CONFIGURATIONS Release
	RUNTIME DESTINATION bin
)
