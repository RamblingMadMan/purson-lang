#ifndef PURSON_EXCEPTION_HPP
#define PURSON_EXCEPTION_HPP 1

#include <stdexcept>

#include "location.hpp"

namespace purson{
	class exception: public std::runtime_error{ using std::runtime_error::runtime_error; };
	
	class source_error: public exception{
		public:
			source_error(const class location &loc_, const std::string &msg)
				: exception(msg), m_loc(loc_){}
			
			const class location &location() const noexcept{ return m_loc; }
			
		private:
			 class location m_loc;
	};
}

#endif // !PURSON_EXCEPTION_HPP
