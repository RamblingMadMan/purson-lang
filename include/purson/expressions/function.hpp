#ifndef PURSON_EXPRESSIONS_FUNCTION_HPP
#define PURSON_EXPRESSIONS_FUNCTION_HPP 1

#include <vector>
#include <memory>

#include "base.hpp"

namespace purson{
	class fn_expr: public rvalue_expr{
		public:
			virtual const function_type *value_type() const noexcept = 0;
			virtual std::string_view name() const noexcept = 0;
			virtual const type *return_type() const noexcept = 0;
			virtual const std::vector<std::pair<std::string_view, const type*>> &params() const noexcept = 0;
	};
	
	class return_expr: public expr{
		public:
			return_expr(std::shared_ptr<const rvalue_expr> value_)
				: m_value{value_}{}
				
			const rvalue_expr *value() const noexcept{ return m_value.get(); }
				
		private:
			std::shared_ptr<const rvalue_expr> m_value;
	};
	
	class fn_decl_expr: public fn_expr{
		public:
			fn_decl_expr(std::string_view name_, const function_type *fn_type_, const std::vector<std::pair<std::string_view, const type*>> &params_)
				: m_name{name_}, m_fn_type{fn_type_}, m_params{params_}{}
			
			const function_type *value_type() const noexcept override{ return m_fn_type; }
			
			std::string_view name() const noexcept{ return m_name; }
			const type *return_type() const noexcept{ return m_fn_type->return_type(); }
			const std::vector<std::pair<std::string_view, const type*>> &params() const noexcept{ return m_params; }
			
		private:
			std::string_view m_name;
			const function_type *m_fn_type;
			std::vector<std::pair<std::string_view, const type*>> m_params;
	};
	
	class fn_def_expr: public fn_expr{
		public:
			fn_def_expr(std::shared_ptr<const fn_decl_expr> decl, std::shared_ptr<const expr> body_)
				: m_decl(decl), m_body(body_){}
			
			const function_type *value_type() const noexcept{ return m_decl->value_type(); }
			
			std::string_view name() const noexcept{ return m_decl->name(); }
			const type *return_type() const noexcept{ return m_decl->return_type(); }
			const std::vector<std::pair<std::string_view, const type*>> &params() const noexcept{ return m_decl->params(); }
			
			const expr *body() const noexcept{ return m_body.get(); }
			
		private:
			std::shared_ptr<const fn_decl_expr> m_decl;
			std::shared_ptr<const expr> m_body;
	};
}

#endif // !PURSON_EXPRESSIONS_FUNCTION_HPP
