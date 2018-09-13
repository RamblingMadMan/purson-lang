#include <functional>

#include "fmt/printf.h"

#include "parser.hpp"
#include "purson/expressions/var.hpp"
#include "purson/expressions/type.hpp"

namespace purson{
	std::shared_ptr<const rvalue_expr> parse_inner(delim_fn_t delim_fn, token_iterator_t &it, token_iterator_t end, parser_scope &scope){
		if(delim_fn(*it)) return nullptr;
		switch(it->type()){
			case token_type::integer:
			case token_type::real:
			case token_type::string:{
				auto &&lit = *it;
				return parse_literal(lit, delim_fn, ++it, end, scope);
			}
			
			case token_type::id:{
				auto &&id = *it;
				return parse_id(id, delim_fn, ++it, end, scope);
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
	
	std::shared_ptr<const rvalue_expr> parse_value(delim_fn_t delim_fn, token_iterator_t &it, token_iterator_t end, parser_scope &scope){
		if(delim_fn(*it)) return nullptr;
		else if(it == end)
			throw parser_error{(--it)->loc(), fmt::format("unexpected end of source. expected a value")};

		switch(it->type()){
			case token_type::integer:
			case token_type::real:
			case token_type::string:{
				auto &&lit = *it++;
				return parse_literal(lit, delim_fn, it, end, scope);
			}

			case token_type ::id:{
				auto &&id = *it++;
				if(std::isupper(id.str()[0])){
					auto ty = scope.get_type(id.str());
					if(!ty)
						throw parser_error{id.loc(), fmt::format("no such type '{}'", id.str())};

					auto type_ref = std::make_shared<const type_ref_expr>(std::string(id.str()), ty, scope.typeset()->type_());

					if(delim_fn(*it))
						return type_ref;
					else
						return parse_leading_value(std::move(type_ref), delim_fn, it, end, scope);
				}
				else
					return parse_id(id, delim_fn, it, end, scope);
			}
			
			case token_type::op:{
				auto &&op = *it++;
				return parse_unary_op(op, delim_fn, it, end, scope);
			}
			
			case token_type::keyword:{
				auto &&kw = *it++;
				if(kw.str() == "match")
					return parse_match(kw, delim_fn, it, end, scope);
				else if(kw.str() == "fn")
					return parse_fn(kw, fn_visibility::local, fn_linkage::purson, delim_fn, it, end, scope);
				else if(kw.str() == "var")
					return parse_var(kw, delim_fn, it, end, scope);
				else
					throw parser_error{it->loc(), "unexpected keyword for value expression"};
			}
			
			default:
				throw parser_error{it->loc(), fmt::format("unexpected token '{}' for value expression", it->str())};
		}
	}

	std::shared_ptr<const lvalue_expr> parse_type(const token &type, delim_fn_t delim_fn, token_iterator_t &it, token_iterator_t end, parser_scope &scope){
		switch(it->type()){
			case token_type::id:{
				if(!std::isupper(it->str()[0]))
					throw parser_error{it->loc(), "type names must begin with a capital letter"};

				auto ty = scope.get_type(it->str());
				if(ty)
					throw parser_error{it->loc(), fmt::format("type with name '{}' already exists", it->str())};

				auto type_name = it;

				++it;

				if(it->str() == "=>"){
					auto rhs_tok = ++it;
					auto rhs = parse_value(delim_fn, it, end, scope);
					auto def = std::make_shared<const type_def_expr>(std::string(it->str()), std::move(rhs), scope.typeset());
					scope.set_type(type_name->str(), def->defined());
					return def;
				}
				else
					throw parser_error{it->loc(), fmt::format("expected return operator after type name '{}'", type_name->str())};
			}

			default:
				throw parser_error{it->loc(), fmt::format("unexpected token '{}' after type keyword", it->str())};
		}
	}
	
	std::shared_ptr<const rvalue_expr> parse_leading_value(std::shared_ptr<const rvalue_expr> val, delim_fn_t delim_fn, token_iterator_t &it, token_iterator_t end, parser_scope &scope){
		switch(it->type()){
			case token_type::op:{
				auto &&op = *it;
				return parse_binary_op(std::move(val), op, delim_fn, ++it, end, scope);
			}
			
			case token_type::bracket:{
				if(it->str() == "("){
					auto fn_ref = std::dynamic_pointer_cast<const fn_ref_expr>(val);
					if(fn_ref){
						auto args_delim = [](const token &tok){ return tok.str() == ")"; };
						auto args_expr = parse_inner(args_delim, ++it, end, scope);
						std::vector<std::shared_ptr<const rvalue_expr>> args;
						++it; // eat closing ')'
						
						if(auto binop = std::dynamic_pointer_cast<const binary_op_expr>(args_expr)){
							do_binop:
							if(binop && (binop->operator_().op_type() == operator_type::comma)){
								args.push_back(binop->lhs());
								if(auto new_binop = std::dynamic_pointer_cast<const binary_op_expr>(binop->rhs())){
									if(new_binop->operator_().op_type() == operator_type::comma){
										binop = new_binop;
										goto do_binop;
									}
								}
								else
									args.push_back(binop->rhs());
							}
							else
								args.push_back(std::move(binop));
						} else
							args.push_back(std::move(args_expr));
						
						std::shared_ptr<const fn_expr> best_match;

						if(fn_ref->fns().size() != 1){
							for(auto &&fn : fn_ref->fns()){
								if(fn->params().size() != args.size())
									continue;

								bool good_match = true;
								bool perfect_match = true;

								for(std::size_t i = 0; i < fn->params().size(); i++){
									auto param_ty = fn->params()[i].second;
									if(param_ty != args[i]->value_type()){
										if(param_ty){
											good_match = false;
											break;
										} else
											perfect_match = false;
									}
								}

								if(good_match){
									best_match = fn;
									if(perfect_match){
										best_match = fn;
										break;
									} else
										throw parser_error{it->loc(),
														   "only perfect function calls are currently supported (exact arity and types)"};
								}
							}
						} else
							best_match = fn_ref->fns()[0];
						
						if(!best_match)
							throw parser_error{it->loc(), "no function with that id"};
						
						auto ret = std::make_shared<const fn_call_expr>(std::move(best_match), args);
						if(delim_fn(*it))
							return std::static_pointer_cast<const rvalue_expr>(std::move(ret));
						else
							return parse_leading_value(std::move(ret), delim_fn, it, end, scope);
					}
					else
						throw parser_error{it->loc(), "unexpected opening parenthesis"};
				}
				else
					throw parser_error{it->loc(), "unexpected bracket after value"};
			}
			
			default:
				throw parser_error{it->loc(), "unexpected token after value expression"};
		}
	}
	
	std::shared_ptr<const expr> parse_top(token_iterator_t &it, token_iterator_t end, parser_scope &scope){
		switch(it->type()){
			case token_type::end: return parse_top(++it, end, scope);

			case token_type::keyword:{
				auto &&kw = *it;
				return parse_keyword(kw, default_delim, ++it, end, scope);
			}
			
			default:
				throw parser_error{it->loc(), fmt::format("unexpected token '{}' at top level", it->str())};
		}
	}
	
	std::shared_ptr<const rvalue_expr> parse_unary_op(const token &op, delim_fn_t delim_fn, token_iterator_t &it, token_iterator_t end, parser_scope &scope){
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
				return std::make_shared<const unary_op_expr>(*op_opt, val);
			}
			
			default:
				throw parser_error{it->loc(), "unexpected token after unary operator"};
		}
	}
	
	std::shared_ptr<const rvalue_expr> parse_binary_op(std::shared_ptr<const rvalue_expr> lhs, const token &op, delim_fn_t delim_fn, token_iterator_t &it, token_iterator_t end, parser_scope &scope){
		auto op_opt = op_type_from_str(op.str());
		if(!op_opt)
			throw parser_error{op.loc(), "invalid operator"};
		
		if(it == end) throw parser_error{op.loc(), "unexpected end of tokens after operator"};
		else if(delim_fn(*it)) throw parser_error{op.loc(), "expected value after binary operator"};

		auto rhs = parse_value(delim_fn, it, end, scope);
		return std::make_shared<const binary_op_expr>(*op_opt, std::move(lhs), std::move(rhs));
	}
	
	std::shared_ptr<const rvalue_expr> parse_literal(const token &lit, delim_fn_t delim_fn, token_iterator_t &it, token_iterator_t end, parser_scope &scope){
		if(it == end) throw parser_error{lit.loc(), "unexpected end of tokens after literal"};
		
		std::shared_ptr<const rvalue_expr> ret;
		
		switch(lit.type()){
			case token_type::integer: ret = std::make_shared<integer_literal_expr>(lit.str(), scope.typeset()); break;
			case token_type::real: ret = std::make_shared<real_literal_expr>(lit.str(), scope.typeset()); break;
			case token_type::string: ret = std::make_shared<string_literal_expr>(lit.str(), scope.typeset()); break;
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
	
	std::shared_ptr<const rvalue_expr> parse_id(const token &id, delim_fn_t delim_fn, token_iterator_t &it, token_iterator_t end, parser_scope &scope){
		if(it == end) throw parser_error{id.loc(), "unexpected end of tokens after identifier"};
		
		if(auto var = scope.get_var(id.str())){
			auto ref = std::make_shared<const var_ref_expr>(std::move(var));
			if(delim_fn(*it))
				return ref;
			else
				return parse_leading_value(std::move(ref), delim_fn, it, end, scope);
		}
		else if(auto fns = scope.get_fn(id.str()); fns.size()){
			auto ret = std::make_shared<const fn_ref_expr>(std::move(fns));
			if(delim_fn(*it))
				return ret;
			else
				return parse_leading_value(std::move(ret), delim_fn, it, end, scope);
		}
		else
			throw parser_error{id.loc(), "id does not refer to a variable"};
	}
	
	std::shared_ptr<const rvalue_expr> parse_match(const token &match, delim_fn_t delim_fn, token_iterator_t &it, token_iterator_t end, parser_scope &scope){
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
	
	std::shared_ptr<const rvalue_expr> parse_keyword(const token &kw, delim_fn_t delim_fn, token_iterator_t &it, token_iterator_t end, parser_scope &scope){
		switch(kw.str()[0]){
			case 'f':{
				if(kw.str() == "fn") return parse_fn(kw, fn_visibility::local, fn_linkage::purson, delim_fn, it, end, scope);
				break;
			}
			
			case 'v':{
				if(kw.str() == "var") return parse_var(kw, delim_fn, it, end, scope);
				break;
			}

			case 't':{
				if(kw.str() == "type") return parse_type(kw, delim_fn, it, end, scope);
				break;
			}
			
			case 'm':{
				if(kw.str() == "match") return parse_match(kw, delim_fn, it, end, scope);
				break;
			}

			case 'i':{
				if(kw.str() == "import"){
					auto visibility = fn_visibility::imported;
					auto linkage = fn_linkage::purson;

					if(it->str() == "["){
						++it;
						if(it->str() == "C"){
							linkage = fn_linkage::C;
							++it;
							if(it->str() != "]")
								throw parser_error{it->loc(), "expected closing square bracket after linkage specifier"};

							++it;
						}
						else if(it->str() == "purson"){
							linkage = fn_linkage::purson;
							++it;
							if(it->str() != "]")
								throw parser_error{it->loc(), "expected closing square bracket after linkage specifier"};

							++it;
						}
						else
							throw parser_error{it->loc(), "expected linkage specifier"};
					}

					auto &&fn_kw = *it;

					if(it->str() == "fn")
						return parse_fn(fn_kw, visibility, linkage, delim_fn, ++it, end, scope);
					else
						throw parser_error{it->loc(), fmt::format("expected function declaration after visibility specifier. got '{}'", it->str())};
				}
				break;
			}

			case 'e':{
				if(kw.str() == "export"){
					auto visibility = fn_visibility::exported;
					auto linkage = fn_linkage::purson;

					if(it->str() == "["){
						++it;
						if(it->str() == "C"){
							linkage = fn_linkage::C;
							++it;
							if(it->str() != "]")
								throw parser_error{it->loc(), "expected closing square bracket after linkage specifier"};

							++it;
						}
						else if(it->str() == "purson"){
							linkage = fn_linkage::purson;
							++it;
							if(it->str() != "]")
								throw parser_error{it->loc(), "expected closing square bracket after linkage specifier"};

							++it;
						}
						else
							throw parser_error{it->loc(), "expected linkage specifier"};
					}

					auto &&fn_kw = *it;

					if(it->str() == "fn")
						return parse_fn(fn_kw, visibility, linkage, delim_fn, ++it, end, scope);
					else
						throw parser_error{it->loc(), fmt::format("expected function declaration after visibility specifier. got '{}'", it->str())};
				}
				break;
			}
			
			default: break;
		}
		
		throw parser_error{kw.loc(), "unimplemented keyword"};
	}
	
	std::shared_ptr<const lvalue_expr> parse_var(const token &var, delim_fn_t delim_fn, token_iterator_t &it, token_iterator_t end, parser_scope &scope){
		if(it == end)
			throw parser_error{var.loc(), "unexpected end of tokens after var keyword"};
		else if(delim_fn(*it))
			throw parser_error{it->loc(), "unexpected deliminator after var keyword"};
		else if(it->type() != token_type::id)
			throw parser_error{it->loc(), "expected identifier after var keyword"};

		auto &&id = *it++;
		if(scope.typeset()->get(id.str()))
			throw parser_error{id.loc(), "type name can not be used for function name"};

		const type *ty = nullptr;

		if(it->str() == ":"){
			++it;
			if(it == end)
				throw parser_error{id.loc(), "unexpected end of tokens after type specifier"};
			else if(delim_fn(*it))
				throw parser_error{it->loc(), "unexpected end of variable declaration"};
			else if(it->type() != token_type::id)
				throw parser_error{it->loc(), "expected type name after type specifier"};

			ty = scope.typeset()->get(it->str());
			if(!ty)
				throw parser_error{var.loc(), "unknown type name"};

			++it;
		}

		if(delim_fn(*it)){
			if(!ty)
				throw parser_error{var.loc(), "can not have untyped uninitialized variables"};
			return std::make_shared<const var_decl_expr>(id.str(), ty, true);
		}
		else if(it->str() == "="){
			auto return_expr = parse_value(delim_fn, ++it, end, scope);
			return std::make_shared<var_def_expr>(id.str(), true, std::move(return_expr));
		}
		else
			throw parser_error{it->loc(), "only variable definitions currently supported"};
	}
	
	std::vector<std::shared_ptr<const expr>> parse(
		std::string_view ver,
		const std::vector<token> &tokens,
		const typeset *types
	){
		if(!types) types = purson::types(ver);
		parser_scope scope(types);
		
		std::vector<std::shared_ptr<const expr>> ret;
		
		auto it_end = end(tokens);
		
		for(auto it = begin(tokens); it != it_end; ++it){
			auto expr_ = parse_top(it, it_end, scope);
			if(expr_)
				ret.push_back(expr_);
		}
		
		return ret;
	}
	
	std::vector<std::shared_ptr<const expr>> parse_repl(
		std::string_view ver,
		const std::vector<token> &tokens,
		const typeset *types
	){
		if(!types) types = purson::types(ver);
		parser_scope scope(types);
		
		std::vector<std::shared_ptr<const expr>> ret;
		
		auto it_end = end(tokens);
		
		for(auto it = begin(tokens); it != it_end; ++it){
			auto expr_ = parse_inner(default_delim, it, it_end, scope);
			if(expr_)
				ret.push_back(expr_);
		}
		
		return ret;
	}
}
