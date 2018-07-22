#ifndef PURSON_EXPRESSIONS_FUNCTION_HPP
#define PURSON_EXPRESSIONS_FUNCTION_HPP 1

#include <vector>
#include <memory>

#include "base.hpp"

namespace purson{
	class fn_expr: public lvalue_expr{
		public:
			virtual const function_type *value_type() const noexcept = 0;

			virtual bool is_mutable() const noexcept override{ return false; }
			
			virtual const type *return_type() const noexcept = 0;
			virtual const std::vector<std::pair<std::string_view, const type*>> &params() const noexcept = 0;
	};
	
	class fn_ref_expr: public lvalue_expr{
		public:
			explicit fn_ref_expr(std::vector<std::shared_ptr<const fn_expr>> fns_)
				: m_fns(std::move(fns_)){}
			
			std::string_view name() const noexcept override{ return m_fns[0]->name(); }
			bool is_mutable() const noexcept override{ return false; }
			
			const std::vector<std::shared_ptr<const fn_expr>> &fns() const noexcept{ return m_fns; }
			
			const type *value_type() const noexcept override{ return nullptr; }
			
		private:
			std::vector<std::shared_ptr<const fn_expr>> m_fns;
	};
	
	class fn_call_expr: public rvalue_expr{
		public:
			fn_call_expr(std::shared_ptr<const fn_expr> fn_, const std::vector<std::shared_ptr<const rvalue_expr>> &args)
				: m_fn(std::move(fn_)), m_args(args){}
			
			const std::shared_ptr<const fn_expr> &fn() const noexcept{ return m_fn; }
			const std::vector<std::shared_ptr<const rvalue_expr>> &args() const noexcept{ return m_args; }
			
			const type *value_type() const noexcept override{ return m_fn->return_type(); }
			
		private:
			std::shared_ptr<const fn_expr> m_fn;
			std::vector<std::shared_ptr<const rvalue_expr>> m_args;
	};
	
	class return_expr: public rvalue_expr{
		public:
			explicit return_expr(std::shared_ptr<const rvalue_expr> value_)
				: m_value{std::move(value_)}{}
				
			const rvalue_expr *value() const noexcept{ return m_value.get(); }

			const type *value_type() const noexcept override{ return m_value->value_type(); }

		private:
			std::shared_ptr<const rvalue_expr> m_value;
	};
	
	class block_expr: public rvalue_expr{
		public:
			explicit block_expr(std::vector<std::shared_ptr<const rvalue_expr>> exprs_, const type *ret_ty_ = nullptr)
				: m_exprs(std::move(exprs_)), m_ret_ty(ret_ty_){}
				
			explicit block_expr(std::vector<std::shared_ptr<const rvalue_expr>> &&exprs_)
				: m_exprs(std::move(exprs_)){}
			
			const type *value_type() const noexcept override{ return m_ret_ty; }
			
			const std::vector<std::shared_ptr<const rvalue_expr>> &exprs() const noexcept{ return m_exprs; }
			
		private:
			std::vector<std::shared_ptr<const rvalue_expr>> m_exprs;
			const type *m_ret_ty;
	};
	
	class fn_decl_expr: public fn_expr{
		public:
			fn_decl_expr(std::string_view name_, const function_type *fn_type_, std::vector<std::pair<std::string_view, const type*>> params_)
				: m_name{name_}, m_fn_type{fn_type_}, m_params{std::move(params_)}{}
			
			const function_type *value_type() const noexcept override{ return m_fn_type; }
			
			std::string_view name() const noexcept override{ return m_name; }
			const type *return_type() const noexcept override{ return m_fn_type->return_type(); }
			const std::vector<std::pair<std::string_view, const type*>> &params() const noexcept override{ return m_params; }
			
		private:
			std::string_view m_name;
			const function_type *m_fn_type;
			std::vector<std::pair<std::string_view, const type*>> m_params;
	};
	
	class fn_def_expr: public fn_expr{
		public:
			fn_def_expr(std::shared_ptr<const fn_decl_expr> decl, std::shared_ptr<const expr> body_)
				: m_decl(std::move(decl)), m_body(std::move(body_)){}
			
			const function_type *value_type() const noexcept override{ return m_decl->value_type(); }
			
			const fn_decl_expr *decl() const noexcept{ return m_decl.get(); }
			
			std::string_view name() const noexcept override{ return m_decl->name(); }
			const type *return_type() const noexcept override{ return m_decl->return_type(); }
			const std::vector<std::pair<std::string_view, const type*>> &params() const noexcept override{ return m_decl->params(); }
			
			const expr *body() const noexcept{ return m_body.get(); }
			
		private:
			std::shared_ptr<const fn_decl_expr> m_decl;
			std::shared_ptr<const expr> m_body;
	};
}

#endif // !PURSON_EXPRESSIONS_FUNCTION_HPP
