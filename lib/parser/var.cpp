#include "../parser.hpp"

namespace purson{
	std::shared_ptr<const lvalue_expr> parse_var(
		const token &var,
		delim_fn_t delim_fn,
		token_iterator_t &it,
		token_iterator_t end,
		parser_scope &scope
	){
		if(it == end)
			throw parser_error{var.loc(), "unexpected end of tokens after var keyword"};
		else if(delim_fn(*it))
			throw parser_error{it->loc(), "unexpected deliminator after var keyword"};
		else if(it->type() != token_type::id)
			throw parser_error{it->loc(), "expected identifier after var keyword"};

		bool is_mutable = true;
		if(var.str() == "var"){}
		else if(var.str() == "let"){ is_mutable = false; }
		else{ throw parser_error{var.loc(), "unknown variable declaration keyword"}; }

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
			else if(!is_mutable)
				throw parser_error{it->loc(), "can not have valueless constant"};

			return std::make_shared<const var_decl_expr>(id.str(), ty, is_mutable);
		} else if(it->str() == "="){
			auto return_expr = parse_value(delim_fn, ++it, end, scope);
			return std::make_shared<var_def_expr>(id.str(), is_mutable, std::move(return_expr));
		} else
			throw parser_error{it->loc(), "only variable definitions currently supported"};
	}
}
