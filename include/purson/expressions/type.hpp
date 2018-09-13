#ifndef PURSON_EXPRESSIONS_TYPE_HPP
#define PURSON_EXPRESSIONS_TYPE_HPP 1

#include <memory>
#include <vector>

#include "../types.hpp"

namespace purson{
	class type_ref_expr: public lvalue_expr{
		public:
			type_ref_expr(const std::string &name_, const type *ty, const type_type *ty_ty)
				: m_name(name_), m_ty(ty), m_ty_ty(ty_ty){}

			std::string_view name() const noexcept override{ return m_name; }
			bool is_mutable() const noexcept override{ return false; }

			const type_type *value_type() const noexcept override{ return m_ty_ty; }

			const type *referenced() const noexcept{ return m_ty; }

		private:
			std::string m_name;
			const type *m_ty;
			const type_type *m_ty_ty;
	};

	class type_block_expr: public rvalue_expr{
		public:
			type_block_expr(const std::vector<std::shared_ptr<const rvalue_expr>> &exprs_, const typeset *types)
				: m_exprs(exprs_), m_ty_ty(types->type_()){}

			const type_type *value_type() const noexcept override{ return m_ty_ty; }

			const std::vector<std::shared_ptr<const rvalue_expr>> &exprs() const noexcept{ return m_exprs; }

		private:
			std::vector<std::shared_ptr<const rvalue_expr>> m_exprs;
			const type_type *m_ty_ty;
	};

	class type_def_expr: public lvalue_expr{
		public:
			type_def_expr(const std::string &name_, std::shared_ptr<const rvalue_expr> type_expr_, const typeset *types){
				if(auto ref = std::dynamic_pointer_cast<const type_ref_expr>(type_expr_))
					m_ty = ref->referenced();
				else if(auto block = std::dynamic_pointer_cast<const type_block_expr>(type_expr_))
					m_ty = solve_type(block.get(), types);

				m_ty_ty = types->type_();
			}

			std::string_view name() const noexcept override{ return m_name; }
			bool is_mutable() const noexcept override{ return false; }

			const type_type *value_type() const noexcept override{ return m_ty_ty; }

			const type *defined() const noexcept{ return m_ty; }

		private:
			std::string m_name;
			const type *m_ty = nullptr;
			const type_type *m_ty_ty;
	};
}

#endif // !PURSON_EXPRESSIONS_TYPE_HPP
