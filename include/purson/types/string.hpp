#ifndef PURSON_AXIOMS_STRING_HPP
#define PURSON_AXIOMS_STRING_HPP 1

#include "base.hpp"

namespace purson{
	struct ascii_string_axiom: string_axiom{};
	struct utf_string_axiom: string_axiom{};
	
	struct utf8_string_axiom: utf_string_axiom{};
	struct utf16_string_axiom: utf_string_axiom{};
	struct utf32_string_axiom: utf_string_axiom{};
}

#endif // !PURSON_AXIOMS_STRING_HPP
