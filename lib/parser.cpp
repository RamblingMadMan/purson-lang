#include <functional>

#include "fmt/printf.h"

#include "parser.hpp"

namespace purson{
	std::shared_ptr<expr> parse_inner(delim_fn_t delim_fn, token_iterator_t &it, token_iterator_t end, parser_scope &scope){
		switch(it->type()){
			case token_type::integer:
			case token_type::real:
			case token_type::string:
			case token_type::ch:{
				auto &&lit = *it;
				return parse_literal(lit, delim_fn, ++it, end, scope);
			}
			
			case token_type::op:{
				auto &&op = *it;
				return parse_unary_op(op, delim_fn, ++it, end, scope);
			}
			
			default:
				throw parser_error{it->loc(), "unexpected token"};
		}
	}
	
	std::shared_ptr<rvalue_expr> parse_value(delim_fn_t delim_fn, token_iterator_t &it, token_iterator_t end, parser_scope &scope){
		switch(it->type()){
			case token_type::integer:
			case token_type::real:
			case token_type::string:
			case token_type::ch:{
				auto &&lit = *it;
				return parse_literal(lit, delim_fn, ++it, end, scope);
			}
			
			case token_type::op:{
				auto &&op = *it;
				return parse_unary_op(op, delim_fn, ++it, end, scope);
			}
			
			default:
				throw parser_error{it->loc(), "unexpected token"};
		}
	}
	
	std::shared_ptr<expr> parse_top(token_iterator_t &it, token_iterator_t end, parser_scope &scope){
		switch(it->type()){
			case token_type::keyword:{
				auto &&kw = *it;
				return parse_keyword(kw, default_delim, ++it, end, scope);
			}
			
			default:
				throw parser_error{it->loc(), "unexpected token"};
		}
	}
	
	std::shared_ptr<rvalue_expr> parse_unary_op(const token &op, delim_fn_t delim_fn, token_iterator_t &it, token_iterator_t end, parser_scope &scope){
		auto op_opt = op_type_from_str(op.str());
		if(!op_opt)
			throw parser_error{op.loc(), "invalid operator"};
		
		if(it == end) throw parser_error{op.loc(), "unexpected end of tokens after operator"};
		else if(delim_fn(*it)) throw parser_error{op.loc(), "expected value after unary operator"};
		
		switch(it->type()){
			case token_type::integer:
			case token_type::real:{
				auto &&lit = *it;
				auto val = parse_literal(lit, delim_fn, ++it, end, scope);
				return std::make_shared<unary_op_expr>(*op_opt, val);
			}
			
			default:
				throw parser_error{it->loc(), "unexpected token after unary operator"};
		}
	}
	
	std::shared_ptr<rvalue_expr> parse_binary_op(std::shared_ptr<rvalue_expr> lhs, const token &op, delim_fn_t delim_fn, token_iterator_t &it, token_iterator_t end, parser_scope &scope){
		auto op_opt = op_type_from_str(op.str());
		if(!op_opt)
			throw parser_error{op.loc(), "invalid operator"};
		
		if(it == end) throw parser_error{op.loc(), "unexpected end of tokens after operator"};
		else if(delim_fn(*it)) throw parser_error{op.loc(), "expected value after binary operator"};
		
		switch(it->type()){
			case token_type::integer:
			case token_type::real:{
				auto &&lit = *it;
				auto val = parse_literal(lit, delim_fn, ++it, end, scope);
				return std::make_shared<binary_op_expr>(*op_opt, std::move(lhs), std::move(val));
			}
			
			default:
				throw parser_error{it->loc(), "unexpected token after binary operator"};
		}
	}
	
	std::shared_ptr<rvalue_expr> parse_literal(const token &lit, delim_fn_t delim_fn, token_iterator_t &it, token_iterator_t end, parser_scope &scope){
		std::shared_ptr<rvalue_expr> ret;
		
		switch(lit.type()){
			case token_type::integer: ret = std::make_shared<integer_literal_expr>(lit.str(), scope.typeset()); break;
			case token_type::real: ret = std::make_shared<real_literal_expr>(lit.str(), scope.typeset()); break;
			default:
				throw parser_error{it->loc(), "unexpected token for literal"};
		}
		
		if(it == end) throw parser_error{lit.loc(), "unexpected end of tokens after literal"};
		else if(delim_fn(*it)) return ret;
			
		switch(it->type()){
			case token_type::op:{
				auto &&op = *it;
				return parse_binary_op(std::move(ret), op, delim_fn, ++it, end, scope);
			}
			
			default:
				throw parser_error{it->loc(), "unexpected token after real literal"};
		}
	}
	
	std::shared_ptr<expr> parse_keyword(const token &kw, delim_fn_t delim_fn, token_iterator_t &it, token_iterator_t end, parser_scope &scope){
		switch(kw.str()[0]){
			case 'f':{
				if(kw.str() == "fn") return parse_fn(kw, delim_fn, it, end, scope);
				break;
			}
			
			case 'v':{
				if(kw.str() == "var") return parse_var(kw, delim_fn, it, end, scope);
				break;
			}
			
			default: break;
		}
		
		throw parser_error{kw.loc(), "unrecognized keyword"};
	}
	
	std::shared_ptr<lvalue_expr> parse_var(const token &var, delim_fn_t delim_fn, token_iterator_t &it, token_iterator_t end, parser_scope &scope){
		throw parser_error{var.loc(), "variables not implemented"};
	}
	
	std::vector<std::shared_ptr<expr>> parse(
		std::string_view ver,
		const std::vector<token> &tokens
	){
		auto types = purson::types(ver);
		parser_scope scope(types);
		
		std::vector<std::shared_ptr<expr>> ret;
		
		auto it_end = end(tokens);
		
		for(auto it = begin(tokens); it != it_end; ++it){
			ret.push_back(parse_top(it, it_end, scope));
		}
		
		return ret;
	}
}
