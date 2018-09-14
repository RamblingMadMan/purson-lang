#ifndef PURSON_PARSER_HPP
#define PURSON_PARSER_HPP 1

#include <vector>
#include <memory>

#include "token.hpp"
#include "exception.hpp"
#include "expressions/function.hpp"
#include "types.hpp"

namespace purson{
	class parser_error: public source_error{ using source_error::source_error; };
	
	/**
	 * Parse tokens into AST
	 * 
	 * @param[in] ver version string
	 * @param[in] tokens lexed source code
	 * @returns AST of parsed source code
	 **/
	
	std::vector<std::shared_ptr<const expr>> parse(
		std::string_view ver,
		const std::vector<token> &tokens,
		const typeset *types = nullptr
	);
	
	std::vector<std::shared_ptr<const expr>> parse_repl(
		std::string_view ver,
		const std::vector<token> &tokens,
		const typeset *types = nullptr
	);

	inline std::vector<std::shared_ptr<const fn_expr>> getFns(const std::vector<std::shared_ptr<const expr>> &exprs){
		std::vector<std::shared_ptr<const fn_expr>> ret;
		for(auto &&expr : exprs){
			if(auto fn = std::dynamic_pointer_cast<const fn_expr>(expr))
				ret.push_back(std::move(fn));
		}
		return ret;
	}
}

#endif // !PURSON_PARSER_HPP
