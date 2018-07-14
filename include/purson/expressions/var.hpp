#ifndef PURSON_EXPRESSIONS_VAR_HPP
#define PURSON_EXPRESSIONS_VAR_HPP 1

#include <memory>

#include "base.hpp"

namespace purson{
	class var_decl_expr: public lvalue_expr{
		public:
			var_decl_expr(std::string_view name, bool is_mutable_ = true, const type *value_type_ = nullptr)
				: m_name{name}, m_value_type{value_type_}{}

			std::string_view name() const noexcept override{ return m_name; }
			bool is_mutable() const noexcept override{ return m_is_mutable; }
			const type *value_type() const noexcept override{ return m_value_type; }

		private:
			std::string_view m_name;
			const type *m_value_type;
			bool m_is_mutable;
	};
}

#endif // !PURSON_EXPRESSIONS_VAR_HPP
