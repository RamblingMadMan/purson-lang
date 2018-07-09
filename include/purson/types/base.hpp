#ifndef PURSON_TYPES_BASE_HPP
#define PURSON_TYPES_BASE_HPP 1

#include <cstddef>

namespace purson{
	//! Base type of all types.
	//! Programming tool and implementation detail
	struct type{
		virtual ~type() = default;
		virtual std::size_t bits() const noexcept = 0;
		virtual std::string_view str() const noexcept = 0;
	};
	
	struct arithmetic_type: virtual type{};
	
	//! base for void types (probably the only)
	struct void_type: virtual type{};

	//! base for boolean types
	struct boolean_type: arithmetic_type{};

	//! base for numeric types
	struct numeric_type: arithmetic_type{};

	//! base for character types
	struct character_type: arithmetic_type{};

	//! base for string types
	struct string_type: virtual type{};
}

#endif // !PURSON_TYPES_BASE_HPP
