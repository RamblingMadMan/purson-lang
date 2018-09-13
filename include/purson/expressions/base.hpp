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
			virtual std::string_view str() const noexcept{ return "no string for this expression :^)"; };
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

	//! an unresolved reference
	class unresolved_identifier_expr: public lvalue_expr{
		public:
			unresolved_identifier_expr(const std::string &id_): m_id(id_){}

			std::string_view name() const noexcept override{ return m_id; }
			bool is_mutable() const noexcept override{ return false; }

			const type *value_type() const noexcept override{ return nullptr; }

		private:
			std::string m_id;
	};
}

#endif // !PURSON_EXPRESSIONS_BASE_HPP
