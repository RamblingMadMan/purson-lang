#ifndef PURSON_AXIOMS_CHARACTER_HPP
#define PURSON_AXIOMS_CHARACTER_HPP 1

#include "base.hpp"

namespace purson{
	struct ascii_character_axiom: character_axiom{};
	struct utf_character_axiom: character_axiom{};
	
	struct utf8_character_axiom: utf_character_axiom{};
	struct utf16_character_axiom: utf_character_axiom{};
	struct utf32_character_axiom: utf_character_axiom{};
}

#endif // !PURSON_AXIOMS_CHARACTER_HPP
