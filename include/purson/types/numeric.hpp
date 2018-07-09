#ifndef PURSON_TYPES_NUMERIC_HPP
#define PURSON_TYPES_NUMERIC_HPP 1

#include "base.hpp"

namespace purson{
	struct integer_type: numeric_type{
		virtual bool is_signed() const noexcept = 0;
	};
	
	struct real_type: numeric_type{
		virtual bool is_ieee754() const noexcept = 0;
	};
	
	struct infinity_type: numeric_type{};
}

#endif // !PURSON_TYPES_NUMERIC_HPP
