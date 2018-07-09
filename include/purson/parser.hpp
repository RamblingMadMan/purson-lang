#ifndef PURSON_PARSER_HPP
#define PURSON_PARSER_HPP 1

#include <vector>
#include <memory>

#include "token.hpp"
#include "exception.hpp"
#include "expressions/base.hpp"

namespace purson{
	class parser_error: public exception{
		public:
			parser_error(const class location &loc, const std::string &msg)
				: exception(msg), m_loc(loc){}
				
			const class location &location() const noexcept{ return m_loc; }
				
		private:
			class location m_loc;
	};
	
	/**
	 * Parse tokens into AST
	 * 
	 * @param[in] ver version string
	 * @param[in] tokens lexed source code
	 * @returns AST of parsed source code
	 **/
	
	std::vector<std::shared_ptr<expr>> parse(
		std::string_view ver,
		const std::vector<token> &tokens
	);
}

#endif // !PURSON_PARSER_HPP
