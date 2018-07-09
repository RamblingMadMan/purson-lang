#ifndef PURSON_EXPRESSION_HPP
#define PURSON_EXPRESSION_HPP 1

#include "types.hpp"

namespace purson{
	class expr{
		public:
			virtual ~expr() = default;
			virtual std::string str() const noexcept = 0;
	};
	
	//! base for all expressions with a value
	class value_expr: public expr{
		public:
			virtual const type *value_type() const noexcept = 0;
	};
	
	//! an rvalue expression e.g. a constant
	class rvalue_expr: public value_expr{};
	
	//! an lvalue expression e.g. a reference
	class lvalue_expr: public rvalue_expr{};
}

#endif // !PURSON_EXPRESSION_HPP
