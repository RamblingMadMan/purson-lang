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
		equ, neq, lt, gt, lte, gte,
		inc, dec,
		set,
		ret, type
	};
	
	inline std::optional<operator_type> op_type_from_str(std::string_view str){
		if(!str.size()) return std::nullopt;
		else if(str.size() == 1){
			switch(str[0]){
				case '+': return operator_type::add;
				case '-': return operator_type::sub;
				case '*': return operator_type::mul;
				case '/': return operator_type::div;
				case '%': return operator_type::div;
				case ':': return operator_type::type;
				case '=': return operator_type::set;
				case '>': return operator_type::gt;
				case '<': return operator_type::lt;
				default: return std::nullopt;
			}
		}
		else if(str.size() == 2){
			switch(str[0]){
				case '+': return str[1] == '+' ? std::make_optional(operator_type::inc) : std::nullopt;
				case '-': return str[1] == '-' ? std::make_optional(operator_type::dec) : std::nullopt;
				
				case '=':{
					switch(str[1]){
						case '>': return operator_type::ret;
						case '=': return operator_type::equ;
						default: return std::nullopt;
					}
				}
				
				default: return std::nullopt;
			}
		}
		else
			return std::nullopt;
	}
	
	class op{
		public:
			op(binary_op_tag_t, std::optional<operator_type> op_ty, const type *lhs, const type *rhs){
				set_op(binary_op, op_ty, lhs, rhs);
			}
			
			op(unary_op_tag_t, std::optional<operator_type> op_ty, const type *operand){
				set_op(unary_op, op_ty, operand);
			}
			
			op(const op&) = default;
			
			op &operator =(const op&) = default;
			
			// fn is_binary() => m_bin;
			bool is_binary() const noexcept{ return m_bin; }
			bool is_unary() const noexcept{ return !m_bin; }
			
			operator_type op_type() const noexcept{ return m_op_type; }
			const type *result_type() const noexcept{ return m_result_type; }
			
		private:
			bool m_bin;
			operator_type m_op_type;
			const type *m_result_type;
			
			void set_op(binary_op_tag_t, std::optional<operator_type> op_ty_opt, const type *lhs, const type *rhs);
			void set_op(unary_op_tag_t, std::optional<operator_type> op_ty_opt, const type *operand);
	};
}

#endif // !PURSON_OPERATOR_HPP
