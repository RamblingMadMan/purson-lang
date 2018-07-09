#ifndef PURSON_TYPES_STRING_HPP
#define PURSON_TYPES_STRING_HPP 1

#include "base.hpp"

namespace purson{
	struct ascii_string_type: string_type{};
	struct utf_string_type: string_type{};
	
	struct utf8_string_type: utf_string_type{};
	struct utf16_string_type: utf_string_type{};
	struct utf32_string_type: utf_string_type{};
}

#endif // !PURSON_TYPES_STRING_HPP
