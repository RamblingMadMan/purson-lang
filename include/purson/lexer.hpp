#ifndef PURSON_LEXER_HPP
#define PURSON_LEXER_HPP 1

#include <vector>

#include "token.hpp"

namespace purson{
	/**
	 * Lex source into tokens
	 * 
	 * @param[in] ver version string
	 * @param[in] name name of source
	 * @param[in] src the code
	 * @returns tokenized source
	 **/
	std::vector<token> lex(std::string_view ver, std::string_view name, std::string_view src);
}

#endif // !PURSON_LEXER_HPP
