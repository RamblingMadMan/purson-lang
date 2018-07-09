#ifndef PURSON_LEXER_HPP
#define PURSON_LEXER_HPP 1

#include <vector>
#include <locale>

#include "exception.hpp"
#include "token.hpp"

namespace purson{
	class lexer_error: public exception{
		public:
			lexer_error(const class location &loc, const std::string &msg)
				: exception(msg), m_loc(loc){}
				
			const class location &location() const noexcept{ return m_loc; }
				
		private:
			class location m_loc;
	};
	
	/**
	 * Lex source into tokens
	 * 
	 * @param[in] ver version string
	 * @param[in] name name of source
	 * @param[in] src the code
	 * @returns tokenized source
	 **/
	std::vector<token> lex(
		std::string_view ver,
		std::string_view name,
		std::string_view src
	);
}

#endif // !PURSON_LEXER_HPP
