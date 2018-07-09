#ifndef PURSON_TYPES_HPP
#define PURSON_TYPES_HPP 1

/**
 * @headerfile types.hpp
 * 
 * Constructs describing the type system of purson
 * 
 * Purson is statically typed, but types are usually inferred.
 **/

namespace purson{
	//! Base type of all types.
	//! Programming tool and implementation detail
	class type{
		public:
			virtual ~type() = default;
	};
	
	class arithmetic_type: public type{};
	
	//! base for void types (probably the only)
	class void_type: public type{};

	//! base for boolean types
	class boolean_type: public arithmetic_type{};

	//! base for numeric types
	class numeric_type: public arithmetic_type{};

	//! base for character types
	class character_type: public arithmetic_type{};

	//! base for string types
	class string_type: public type{};
}

#endif // !PURSON_TYPES_HPP
