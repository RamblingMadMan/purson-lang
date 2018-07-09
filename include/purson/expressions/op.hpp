#ifndef PURSON_EXPRESSIONS_OP_HPP
#define PURSON_EXPRESSIONS_OP_HPP 1

#include "../operator.hpp"

#include "base.hpp"

namespace purson{
	class op_expr: public rvalue_expr{
		public:
			virtual op operator_() const noexcept = 0;
	};
	
	class unary_op_expr: public op_expr{
		public:
			unary_op_expr(operator_type op_ty, const rvalue_expr *operand_)
				: m_operator(unary_op, op_ty, operand_->value_type()), m_operand(operand_){}
			
			const type *value_type() const noexcept override{ return m_operator.result_type(); }
			
			const rvalue_expr *operand() const noexcept{ return m_operand; }
			
			op operator_() const noexcept override{ return m_operator; }
			
		private:
			op m_operator;
			const rvalue_expr *m_operand;
	};
	class binary_op_expr: public op_expr{
		public:
			binary_op_expr(operator_type op_ty, const rvalue_expr *lhs_, const rvalue_expr *rhs_)
				: m_operator(binary_op, op_ty, lhs_->value_type(), rhs_->value_type()), m_lhs(lhs_), m_rhs(rhs_){}
			
			const type *value_type() const noexcept override{ return m_operator.result_type(); }
			
			const rvalue_expr *lhs() const noexcept{ return m_lhs; }
			const rvalue_expr *rhs() const noexcept{ return m_rhs; }
			
			op operator_() const noexcept override{ return m_operator; }
			
		private:
			op m_operator;
			const rvalue_expr *m_lhs, *m_rhs;
	};
}

#endif // !PURSON_EXPRESSIONS_OP_HPP
