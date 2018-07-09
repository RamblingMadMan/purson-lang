#include "purson/operator.hpp"

#include <fmt/format.h>

namespace purson{
	void op::set_op(binary_op_tag_t, std::optional<operator_type> op_ty_opt, const arithmetic_type *lhs, const arithmetic_type *rhs){
		if(!op_ty_opt) throw operator_error{"invalid binary operator '{}'"};
		
		auto &op_ty = op_ty_opt.value();
		
		switch(op_ty){
			case operator_type::inc: throw operator_error{"increment '++' is not a valid binary operator"};
			case operator_type::dec: throw operator_error{"decrement '--' is not a valid bianry operator"};
			default: break;
		}
		
		m_bin = true;
		m_op_type = op_ty;
	}
	
	void op::set_op(unary_op_tag_t, std::optional<operator_type> op_ty_opt, const arithmetic_type *operand){
		if(!op_ty_opt) throw operator_error{"invalid unary operator '{}'"};
		
		auto &op_ty = op_ty_opt.value();
		
		auto throw_invalid = [](auto &&name, auto &&op){
			throw operator_error{fmt::format("{} '{}' is not a valid unary op", name, op)};
		};
		
		switch(op_ty){
			case operator_type::add: throw_invalid("addition", "+");
			case operator_type::sub: throw_invalid("subtraction", "-");
			case operator_type::mul: throw_invalid("multiplication", "*");
			case operator_type::div: throw_invalid("division", "/");
			default: break;
		}
		
		m_bin = false;
		m_op_type = op_ty;
	}
	
	/*
	
	
	*/
}
