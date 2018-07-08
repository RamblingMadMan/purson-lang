#ifndef PURSON_LOCATION_HPP
#define PURSON_LOCATION_HPP 1

#include <cstddef>
#include <string_view>

namespace purson{
	//! Location within source code
	class location{
		public:
			/**
			 * @param[in] src_ name of source
			 * @param[in] line_ line in source
			 * @param[in] col_ column in line
			 * @param[in] len_ number of characters
			 **/
			location(std::string_view src_, std::size_t line_, std::size_t col_, std::size_t len_)
				: m_src(src_), m_line(line_), m_col(col_), m_len(len_){}
				
			location(const location&) = default;
			
			location &operator =(const location&) = default;
			
			//! @returns name of source
			std::string_view src() const noexcept{ return m_src; }
			
			//! @returns line in source
			std::size_t line() const noexcept{ return m_line; }
			
			//! @returns column in line
			std::size_t col() const noexcept{ return m_col; }
			
			//! @returns number of characters
			std::size_t len() const noexcept{ return m_len; }
			
		private:
			std::string_view m_src;
			std::size_t m_line, m_col, m_len;
	};
}

#endif // !PURSON_LOCATION_HPP
