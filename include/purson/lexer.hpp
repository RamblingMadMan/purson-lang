#ifndef PURSON_LEXER_HPP
#define PURSON_LEXER_HPP 1

#include <vector>
#include <locale>

#ifdef _WIN32
// get your shit together windows
#define PURSON_DEFAULT_LOCALE 
#else
#define PURSON_DEFAULT_LOCALE "en_US.UTF-8"
#endif

#include "exception.hpp"
#include "token.hpp"

namespace purson{
	class lexer_error: public exception{
		public:
			template<typename Msg>
			lexer_error(const class location &loc, Msg &&msg)
				: exception(std::forward<Msg>(msg)), m_loc(loc){}
				
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
	 * @param[in] locale locale used for character encoding
	 * @returns tokenized source
	 **/
	std::vector<token> lex(
		std::string_view ver,
		std::string_view name,
		std::string_view src,
		const std::locale &locale = std::locale(PURSON_DEFAULT_LOCALE)
	);
}

#endif // !PURSON_LEXER_HPP
