#include "../parser.hpp"

namespace purson{
	std::shared_ptr<rvalue_expr> parse_fn(const token &fn, delim_fn_t delim_fn, token_iterator_t &it, token_iterator_t end, parser_scope &scope){
		if(it == end)
			throw parser_error{fn.loc(), "unexpected end of tokens after function keyword"};
		else if(delim_fn(*it))
			throw parser_error{it->loc(), "expected parentheses or identifier after function keyword"};
		
		const token *fn_name = nullptr;
		const type *ret_ty = nullptr;
		std::vector<std::pair<const token*, const type*>> params;
		
		if(it->type() == token_type::id){
			// function name
			fn_name = &*it;
			++it;
			
			if(it == end)
				throw parser_error{fn.loc(), "unexpected end of tokens after function name"};
			else if(delim_fn(*it))
				throw parser_error{it->loc(), "expected parentheses after function name"};
		}
		
		if(it->str() == "("){
			// function params
			++it;
			if(it == end)
				throw parser_error{fn.loc(), "unexpected end of tokens after opening function parenthesis"};
			else if(it->str() == ")")
				++it;
			else{
				while((it != end) && (it->str() != ")")){
					if(it == end)
						throw parser_error{fn.loc(), "unexpected end of tokens after opening function parenthesis"};
					else if(delim_fn(*it))
						throw parser_error{it->loc(), "expected closing function parenthesis"};
					
					const token *param_name = nullptr;
					const type *ty = nullptr;
					
					if(it->type() == token_type::id){
						param_name = &*it;
					}
					else if(
						(it->type() == token_type::integer) ||
						(it->type() == token_type::real) ||
						(it->type() == token_type::string) ||
						(it->type() == token_type::ch)
					){
						throw parser_error{it->loc(), "pattern matching isn't implemented yet :^("};
					}
					
					if(!param_name)
						throw parser_error{it->loc(), "expected parameter name"};
					
					++it;
					
					if(it == end)
						throw parser_error{param_name->loc(), "unexpected end of tokens after function parameter"};
					else if(it->str() == ":"){
						++it;
						
						if(it == end)
							throw parser_error{fn.loc(), "unexpected end of tokens after type specifier operator"};
						else if((it->type() != token_type::id) || !(ty = scope.get_type(it->str())))
							throw parser_error{fn.loc(), "expected type after type specifier operator"};
						
						auto &&type_loc = it->loc();
						
						++it;
						if(it == end)
							throw parser_error{type_loc, "unexpected end of tokens after function parameter"};
					}
					
					if(it->str() == ")"){
						++it;
						params.emplace_back(param_name, ty);
						break;
					}
					else if(it->str() == ","){
						++it;
						if((it != end) && (it->str() == ")"))
							throw parser_error{it->loc(), "stray comma in parameter list"};
						
						params.emplace_back(param_name, ty);
						continue;
					}
				}
			}
			
			if(it == end)
				throw parser_error{fn.loc(), "unexpected end of tokens after function parameters"};
			else if(delim_fn(*it))
				throw parser_error{it->loc(), "expected return type or definition after function parameters"};
		}
		
		if(it->str() == "->"){
			// return value axiom
			++it;
			if(it == end)
				throw parser_error{fn.loc(), "unexpected end of tokens after return value axiom operator"};
			else if((it->type() != token_type::id) || !(ret_ty = scope.get_type(it->str())))
				throw parser_error{it->loc(), "expected axiom after return value axiom operator"};
			
			++it;
			if(it == end)
				throw parser_error{fn.loc(), "unexpected end of tokens after function declaration"};
		}
		
		std::vector<std::pair<std::string_view, const type*>> param_info;
		std::vector<const type*> param_types;
		
		param_info.reserve(params.size());
		param_types.reserve(params.size());
		
		for(auto &&param : params){
			param_info.emplace_back(param.first->str(), param.second);
			param_types.push_back(param.second);
		}
		
		if(delim_fn(*it)){
			// simple declaration
			return std::make_shared<fn_decl_expr>(fn_name ? fn_name->str() : "", scope.typeset()->function(ret_ty, param_types), param_info);
		}
		
		parser_scope fn_scope(scope);
		
		auto param_decls = [&params, &param_info, &param_types](){
			std::vector<std::shared_ptr<lvalue_expr>> decls;
			decls.reserve(params.size());
			for(std::size_t i = 0; i < params.size(); i++)
				decls.emplace_back(std::make_shared<var_decl_expr>(param_info[i].first, param_types[i]));
			
			return decls;
		};
		
		if(it->str() == "=>"){
			// expression as return statement
			auto decls = param_decls();
			for(auto &&decl : decls)
				fn_scope.set_var(decl->name(), std::move(decl));
			
			++it;
			auto val_it = it;
			auto ret_val = parse_value(delim_fn, it, end, fn_scope);
			if(ret_ty && (ret_val->value_type() != ret_ty))
				throw parser_error{val_it->loc(), "return value has type different to specified return type"};
			else
				ret_ty = ret_val->value_type();
			
			std::vector<std::string_view> param_names;
			std::vector<const type*> param_types;
			
			param_names.reserve(params.size());
			param_types.reserve(params.size());
			
			for(auto &&param : params){
				param_names.push_back(param.first->str());
				param_types.push_back(param.second);
			}
			
			auto decl = std::make_shared<fn_decl_expr>(fn_name ? fn_name->str() : "", fn_scope.typeset()->function(ret_ty, param_types), param_info);
			auto ret = std::make_shared<return_expr>(std::move(ret_val));
			return std::make_shared<fn_def_expr>(std::move(decl), std::move(ret));
		}
		else if(it->str() == "{"){
			// complex/imperative function body
			throw parser_error{it->loc(), "imperative function style is not implemented yet"};
		}
		else if(it->str() == "="){
			// function alias
			throw parser_error{it->loc(), "function aliases not implemented"};
			
			if(!fn_name)
				throw parser_error{it->loc(), "can not create an unnamed function alias"};
		}
		else
			throw parser_error{fn.loc(), "unexpected expression after function declaration"};
		
		throw parser_error{fn.loc(), "unreachable error. how the hell'd ya get here big boi?"};
	}
}
