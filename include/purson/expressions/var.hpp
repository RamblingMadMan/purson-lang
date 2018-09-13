#ifndef PURSON_EXPRESSIONS_VAR_HPP
#define PURSON_EXPRESSIONS_VAR_HPP 1

#include <memory>

#include "base.hpp"

namespace purson{
	class var_decl_expr: public lvalue_expr{
		public:
			var_decl_expr(std::string_view name_, const type *value_type_ = nullptr, bool is_mutable_ = true)
				: m_name{name_}, m_value_type{value_type_}, m_is_mutable{is_mutable_}{}

			std::string_view name() const noexcept override{ return m_name; }
			bool is_mutable() const noexcept override{ return m_is_mutable; }
			const type *value_type() const noexcept override{ return m_value_type; }

		private:
			std::string_view m_name;
			const type *m_value_type;
			bool m_is_mutable;
	};

	class var_def_expr: public var_decl_expr{
		public:
			var_def_expr(std::string_view name_, bool is_mutable_, std::shared_ptr<const rvalue_expr> value_)
				: var_decl_expr(name_, value_->value_type(), is_mutable_), m_value(std::move(value_)){}

			std::shared_ptr<const rvalue_expr> value() const noexcept{ return m_value; }

		private:
			std::shared_ptr<const rvalue_expr> m_value;
	};

	class var_ref_expr: public lvalue_expr{
		public:
			var_ref_expr(std::shared_ptr<const var_decl_expr> decl_): m_decl{decl_}{}

			std::string_view name() const noexcept override{ return m_decl->name(); }
			bool is_mutable() const noexcept override{ return m_decl->is_mutable(); }
			const type *value_type() const noexcept override{ return m_decl->value_type(); }

		private:
			std::shared_ptr<const var_decl_expr> m_decl;
	};
}

#endif // !PURSON_EXPRESSIONS_VAR_HPP
