find_package(ICU REQUIRED)
find_package(GMP REQUIRED)
find_package(MPFR REQUIRED)
find_package(LLVM 5.0.0 REQUIRED)

link_directories(${LLVM_LIBRARY_DIRS})

set(
	PURSON_SOURCES
	operator.cpp
	lexer.cpp
	types.cpp
	parser.hpp
	parser.cpp
	parser/function.cpp
	llvm.hpp
	llvm.cpp
	module.cpp
	compile_llvm/var.cpp
)

set(
	PURSON_HEADERS

	../include/purson/expressions/var.hpp
	../include/purson/expressions/function.hpp
	../include/purson/expressions/literal.hpp
	../include/purson/expressions/match.hpp
	../include/purson/expressions/op.hpp
	../include/purson/expressions/type.hpp

	../include/purson/types/base.hpp
	../include/purson/types/numeric.hpp
	../include/purson/types/string.hpp

	../include/purson/exception.hpp
	../include/purson/location.hpp
	../include/purson/token.hpp
	../include/purson/types.hpp
	../include/purson/operator.hpp
	../include/purson/expressions.hpp
	../include/purson/lexer.hpp
	../include/purson/parser.hpp
	../include/purson/module.hpp
)

add_library(purson ${PURSON_HEADERS} ${PURSON_SOURCES})

#target_compile_definitions(purson PRIVATE ${LLVM_CXXFLAGS})
target_include_directories(purson PRIVATE ${GMP_INCLUDE_DIRS} ${MPFR_INCLUDE_DIRS} ${LLVM_INCLUDE_DIRS})
target_link_libraries(purson fmt ICU::ICU ${GMP_LIBRARIES} ${MPFR_LIBRARIES} ${LLVM_LIBRARIES})
