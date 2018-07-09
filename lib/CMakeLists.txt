find_package(ICU REQUIRED)
find_package(GMP REQUIRED)
find_package(MPFR REQUIRED)

set(
	PURSON_SOURCES
	operator.cpp
	lexer.cpp
	types.cpp
	parser.cpp
)

add_library(purson ${PURSON_SOURCES})

target_include_directories(purson PRIVATE ${MPFR_INCLUDE_DIRS})
target_link_libraries(purson fmt ICU::ICU Gmp::Gmp ${MPFR_LIBRARIES})
