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
			
			case token_type::keyword:{
				auto &&kw = *it;
				return parse_keyword(kw, default_delim, ++it, end, scope);
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
				auto &&lit = *it++;
				return parse_literal(lit, delim_fn, it, end, scope);
			}
			
			case token_type::op:{
				auto &&op = *it++;
				return parse_unary_op(op, delim_fn, it, end, scope);
			}
			
			case token_type::keyword:{
				auto &&kw = *it++;
				if(kw.str() == "match")
					return parse_match(kw, delim_fn, it, end, scope);
				else
					throw parser_error{it->loc(), "unexpected keyword for value expression"};
			}
			
			default:
				throw parser_error{it->loc(), "unexpected token for value expression"};
		}
	}
	
	std::shared_ptr<rvalue_expr> parse_leading_value(std::shared_ptr<rvalue_expr> val, delim_fn_t delim_fn, token_iterator_t &it, token_iterator_t end, parser_scope &scope){
		switch(it->type()){
			case token_type::op:{
				auto &&op = *it;
				return parse_binary_op(std::move(val), op, delim_fn, ++it, end, scope);
			}
			
			default:
				throw parser_error{it->loc(), "unexpected token after value expression"};
		}
	}
	
	std::shared_ptr<expr> parse_top(token_iterator_t &it, token_iterator_t end, parser_scope &scope){
		switch(it->type()){
			case token_type::keyword:{
				auto &&kw = *it;
				return parse_keyword(kw, default_delim, ++it, end, scope);
			}
			
			default:
				throw parser_error{it->loc(), "unexpected token at top level"};
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
		if(it == end) throw parser_error{lit.loc(), "unexpected end of tokens after literal"};
		
		std::shared_ptr<rvalue_expr> ret;
		
		switch(lit.type()){
			case token_type::integer: ret = std::make_shared<integer_literal_expr>(lit.str(), scope.typeset()); break;
			case token_type::real: ret = std::make_shared<real_literal_expr>(lit.str(), scope.typeset()); break;
			default:
				throw parser_error{it->loc(), "unexpected token for literal"};
		}
		
		if(delim_fn(*it)) return ret;
			
		switch(it->type()){
			case token_type::op:{
				auto &&op = *it;
				return parse_binary_op(std::move(ret), op, delim_fn, ++it, end, scope);
			}
			
			default:
				throw parser_error{it->loc(), "unexpected token after real literal"};
		}
	}
	
	std::shared_ptr<rvalue_expr> parse_id(const token &id, delim_fn_t delim_fn, token_iterator_t &it, token_iterator_t end, parser_scope &scope){
		if(it == end) throw parser_error{id.loc(), "unexpected end of tokens after identifier"};
		
		if(auto var = scope.get_var(id.str())){
			if(delim_fn(*it))
				return var;
			else
				return parse_leading_value(std::move(var), delim_fn, it, end, scope);
		}
		else
			throw parser_error{id.loc(), "id does not refer to a variable"};
	}
	
	std::shared_ptr<rvalue_expr> parse_match(const token &match, delim_fn_t delim_fn, token_iterator_t &it, token_iterator_t end, parser_scope &scope){
		if(it == end) throw parser_error{match.loc(), "unexpected end of tokens after match keyword"};
		else if(it->str() != "(") throw parser_error{it->loc(), "expected match value after match keyword"};
		
		auto match_value = parse_value([](const token &tok){ return tok.str() == ")"; }, ++it, end, scope);
		++it;
		
		if(it->str() != "{") throw parser_error{it->loc(), "expected pattern list after match expression"};
		
		std::vector<std::pair<std::shared_ptr<const rvalue_expr>, std::shared_ptr<const rvalue_expr>>> patterns;
		
		while(1){
			auto pattern = parse_value([](const token &tok){ return tok.str() == "=>"; }, ++it, end, scope);
			auto value = parse_value([](const token &tok){ return ((tok.str() == ",") || (tok.str() == "}")); }, ++it, end, scope);
			patterns.emplace_back(std::move(pattern), std::move(value));
			if(it->str() == "}"){
				++it;
				break;
			}
		}
		
		auto match_expr_ = std::make_shared<match_expr>(match_value, patterns);
		
		if(delim_fn(*it))
			return match_expr_;
		else
			return parse_leading_value(std::move(match_expr_), delim_fn, it, end, scope);
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
			
			case 'm':{
				if(kw.str() == "match") return parse_match(kw, delim_fn, it, end, scope);
				break;
			}
			
			default: break;
		}
		
		throw parser_error{kw.loc(), "unimplemented keyword"};
	}
	
	std::shared_ptr<lvalue_expr> parse_var(const token &var, delim_fn_t delim_fn, token_iterator_t &it, token_iterator_t end, parser_scope &scope){
		throw parser_error{var.loc(), "variables not implemented"};
	}
	
	std::vector<std::shared_ptr<expr>> parse(
		std::string_view ver,
		const std::vector<token> &tokens,
		const typeset *types
	){
		if(!types) types = purson::types(ver);
		parser_scope scope(types);
		
		std::vector<std::shared_ptr<expr>> ret;
		
		auto it_end = end(tokens);
		
		for(auto it = begin(tokens); it != it_end; ++it){
			ret.push_back(parse_top(it, it_end, scope));
		}
		
		return ret;
	}
	
	std::vector<std::shared_ptr<expr>> parse_repl(
		std::string_view ver,
		const std::vector<token> &tokens,
		const typeset *types
	){
		if(!types) types = purson::types(ver);
		parser_scope scope(types);
		
		std::vector<std::shared_ptr<expr>> ret;
		
		auto it_end = end(tokens);
		
		for(auto it = begin(tokens); it != it_end; ++it){
			ret.push_back(parse_inner(default_delim, it, it_end, scope));
		}
		
		return ret;
	}
}
