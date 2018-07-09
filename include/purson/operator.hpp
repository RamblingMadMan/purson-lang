#ifndef PURSON_OPERATOR_HPP
#define PURSON_OPERATOR_HPP 1

#include <functional>
#include <optional>

#include "exception.hpp"
#include "types.hpp"

namespace purson{
	class operator_error: public exception{
		using exception::exception;
	};
	
	struct binary_op_tag_t{} inline constexpr binary_op;
	struct unary_op_tag_t{} inline constexpr unary_op;
	
	enum class operator_type{
		add, sub, mul, div, mod,
		inc, dec
	};
	
	std::optional<operator_type> op_type_from_str(std::string_view str){
		if(!str.size()) return std::nullopt;
		else if(str.size() == 1){
			switch(str[0]){
				case '+': return operator_type::add;
				case '-': return operator_type::sub;
				case '*': return operator_type::mul;
				case '/': return operator_type::div;
				case '%': return operator_type::div;
				default: return std::nullopt;
			}
		}
		else{
			switch(str[0]){
				case '+': return str[1] == '+' ? std::make_optional(operator_type::inc) : std::nullopt;
				case '-': return str[1] == '-' ? std::make_optional(operator_type::dec) : std::nullopt;
				default: return std::nullopt;
			}
		}
	}
	
	class op{
		public:
			op(binary_op_tag_t, std::optional<operator_type> op_ty, const arithmetic_type *lhs, const arithmetic_type *rhs){
				set_op(binary_op, op_ty, lhs, rhs);
			}
			
			template<typename OpVal>
			op(unary_op_tag_t, std::optional<operator_type> op_ty, const arithmetic_type *operand){
				set_op(unary_op, op_ty, operand);
			}
			
			// fn is_binary() => m_bin;
			bool is_binary() const noexcept{ return m_bin; }
			bool is_unary() const noexcept{ return !m_bin; }
			
			operator_type op_type() const noexcept{ return m_op_type; }
			
		private:
			bool m_bin;
			operator_type m_op_type;
			
			void set_op(binary_op_tag_t, std::optional<operator_type> op_ty_opt, const arithmetic_type *lhs, const arithmetic_type *rhs);
			void set_op(unary_op_tag_t, std::optional<operator_type> op_ty_opt, const arithmetic_type *operand);
	};
}

#endif // !PURSON_OPERATOR_HPP
