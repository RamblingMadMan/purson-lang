#ifndef PURSON_TYPES_CHARACTER_HPP
#define PURSON_TYPES_CHARACTER_HPP 1

#include "base.hpp"

namespace purson{
	struct ascii_character_type: character_type{};
	struct utf_character_type: character_type{};
	
	struct utf8_character_type: utf_character_type{};
	struct utf16_character_type: utf_character_type{};
	struct utf32_character_type: utf_character_type{};
}

#endif // !PURSON_TYPES_CHARACTER_HPP
