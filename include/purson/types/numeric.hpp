#ifndef PURSON_TYPES_NUMERIC_HPP
#define PURSON_TYPES_NUMERIC_HPP 1

#include "base.hpp"

namespace purson{
	struct infinity_type: numeric_type{};
	
	struct natural_type: numeric_type{};
	struct integer_type: natural_type{};
	struct rational_type: integer_type{};
	struct real_type: rational_type{};
	struct complex_type: real_type{};
}

#endif // !PURSON_TYPES_NUMERIC_HPP
