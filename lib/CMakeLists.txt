set(
	PURSON_SOURCES
	lexer.cpp
)

add_library(purson ${PURSON_SOURCES})

target_link_libraries(purson fmt)
