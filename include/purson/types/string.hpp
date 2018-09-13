#ifndef PURSON_AXIOMS_STRING_HPP
#define PURSON_AXIOMS_STRING_HPP 1

#include "base.hpp"

namespace purson{
	struct ascii_string_type: string_type{
		virtual char_encoding encoding() const noexcept override{ return char_encoding::ascii; }
	};

	struct utf_string_type: string_type{};
	
	struct utf8_string_type: utf_string_type{
		virtual char_encoding encoding() const noexcept override{ return char_encoding::utf8; }
	};

	struct utf16_string_type: utf_string_type{
		virtual char_encoding encoding() const noexcept override{ return char_encoding::utf16; }
	};

	struct utf32_string_type: utf_string_type{
		virtual char_encoding encoding() const noexcept override{ return char_encoding::utf32; }
	};
}

#endif // !PURSON_AXIOMS_STRING_HPP
