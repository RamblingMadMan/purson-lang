find_package(Readline REQUIRED)

set(
	PURSON_REPL_SOURCES
	main.cpp
)

add_executable(purson-repl ${PURSON_REPL_SOURCES})

target_include_directories(purson-repl PRIVATE ${Readline_INCLUDE_DIR})
target_link_libraries(purson-repl purson fmt ${Readline_LIBRARY})

install(
	TARGETS purson-repl
	CONFIGURATIONS Release
	RUNTIME DESTINATION bin
)
