#ifndef PURSON_EXCEPTION_HPP
#define PURSON_EXCEPTION_HPP 1

#include <stdexcept>

namespace purson{
	class exception: public std::runtime_error{ using std::runtime_error::runtime_error; };
}

#endif // !PURSON_EXCEPTION_HPP
