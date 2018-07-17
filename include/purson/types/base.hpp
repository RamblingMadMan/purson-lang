#ifndef PURSON_TYPES_BASE_HPP
#define PURSON_TYPES_BASE_HPP 1

#include <cstddef>

#include "../exception.hpp"

namespace purson{
	class type_error: public exception{ using exception::exception; };
	
	//! Base class for types
	struct type{
		virtual ~type() = default;
		virtual std::size_t bits() const noexcept = 0;
		virtual std::string_view str() const noexcept = 0;
		
		virtual std::size_t num_members() const noexcept{ return 0; }
		virtual const std::pair<std::string_view, const type*> *members() const noexcept{ return nullptr; }
	};
	
	const type *promote_type(const type *a, const type *b);
	
	struct arithmetic_type: virtual type{};
	
	//! base for function types
	struct function_type: virtual type{
		virtual const type *return_type() const noexcept = 0;
		virtual const std::size_t num_params() const noexcept = 0;
		virtual const type *param_type(std::size_t idx) const noexcept = 0;
	};
	
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
