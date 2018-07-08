#ifndef PURSON_TOKEN_HPP
#define PURSON_TOKEN_HPP 1

#include "location.hpp"

namespace purson{
	//! self-explanatory
	enum class token_type{
		id, keyword, type, op, bracket, integer, real, string, ch, end, eof
	};
	
	/**
	 * A code token
	 **/
	class token{
		public:
			/**
			 * @param[in] type_ type of token
			 * @param[in] str_ token string
			 * @param[in] loc_ location in src code
			 **/
			token(token_type type_, std::string_view str_, const location &loc_)
				: m_type(type_), m_str(str_), m_loc(loc_){}
			
			//! @returns type of the token
			token_type type() const noexcept{ return m_type; }
			
			//! @returns string value
			std::string_view str() const noexcept{ return m_str; }
			
			//! @returns location in source
			const location &loc() const noexcept{ return m_loc; }
			
		private:
			token_type m_type;
			std::string_view m_str;
			location m_loc;
	};
}

#endif // !PURSON_TOKEN_HPP
