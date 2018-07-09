#include "purson/parser.hpp"
#include "purson/types.hpp"
#include "purson/operator.hpp"

#include "purson/expressions.hpp"

namespace purson{
	using token_iterator_t = typename std::vector<token>::const_iterator;
	
	using parse_ret_t = std::shared_ptr<expr>;
	
	parse_ret_t parse_literal(token_iterator_t &it, token_iterator_t end, const typeset *types);
	parse_ret_t parse_op(parse_ret_t operand, token_iterator_t &it, token_iterator_t end, const typeset *types);
	
	parse_ret_t parse_top(token_iterator_t &it, token_iterator_t end, const typeset *types){
		switch(it->type()){
			case token_type::integer:
			case token_type::real:
				return parse_literal(it, end, types); break;
			
			default:
				throw parser_error{it->loc(), "unexpected token"};
		}
	}
	
	parse_ret_t parse_op(std::shared_ptr<rvalue_expr> operand, token_iterator_t &it, token_iterator_t end, const typeset *types){
		auto op_it = it;
		
		auto op_opt = op_type_from_str(it->str());
		if(!op_opt)
			throw parser_error{it->loc(), "invalid operator"};
		
		++it;
		
		if(it == end) throw parser_error{op_it->loc(), "unexpected end of tokens after operator"};
		else if(it->type() == token_type::end)
			return std::make_shared<unary_op_expr>(*op_opt, operand.get());
		
		switch(it->type()){
			default:
				throw parser_error{it->loc(), "unexpected token after real literal"};
		}
	}
	
	parse_ret_t parse_literal(token_iterator_t &it, token_iterator_t end, const typeset *types){
		auto lit_it = it;
		
		std::shared_ptr<rvalue_expr> ret;
		
		switch(lit_it->type()){
			case token_type::integer: ret = std::make_shared<integer_literal_expr>(lit_it->str(), types); break;
			case token_type::real: ret = std::make_shared<real_literal_expr>(lit_it->str(), types); break;
			default:
				throw parser_error{it->loc(), "unexpected token for literal"};
		}
		
		++it;
		
		if(it == end) throw parser_error{lit_it->loc(), "unexpected end of tokens after literal"};
		else if(it->type() == token_type::end)
			return ret;
			
		switch(it->type()){
			case token_type::op: return parse_op(ret, it, end, types);
			
			default:
				throw parser_error{it->loc(), "unexpected token after real literal"};
		}
	}
	
	std::vector<std::shared_ptr<expr>> parse(
		std::string_view ver,
		const std::vector<token> &tokens
	){
		auto types = purson::types(ver);
		
		std::vector<std::shared_ptr<expr>> ret;
		
		auto it_end = end(tokens);
		
		for(auto it = begin(tokens); it != it_end; ++it){
			ret.push_back(parse_top(it, it_end, types));
		}
		
		return ret;
	}
}
