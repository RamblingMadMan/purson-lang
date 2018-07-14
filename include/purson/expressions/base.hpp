#ifndef PURSON_EXPRESSIONS_BASE_HPP
#define PURSON_EXPRESSIONS_BASE_HPP 1

#include "../exception.hpp"
#include "../types/base.hpp"

namespace purson{
	class expr_error: public exception{ using exception::exception; };
	
	//! base for all expressions
	class expr{
		public:
			virtual ~expr() = default;
			//virtual std::string str() const noexcept = 0;
	};
	
	//! base for all expressions with a value
	class value_expr: public expr{
		public:
			virtual const type *value_type() const noexcept = 0;
	};
	
	//! an rvalue expression e.g. a constant
	class rvalue_expr: public value_expr{};
	
	//! an lvalue expression e.g. a reference
	class lvalue_expr: public rvalue_expr{
		public:
			virtual std::string_view name() const noexcept = 0;
			virtual bool is_mutable() const noexcept = 0;
	};
}

#endif // !PURSON_EXPRESSIONS_BASE_HPP
