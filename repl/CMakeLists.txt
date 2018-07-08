set(
	PURSON_REPL_SOURCES
	main.cpp
)

add_executable(purson-repl ${PURSON_REPL_SOURCES})

target_link_libraries(purson-repl purson fmt)
