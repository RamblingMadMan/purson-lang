#ifndef PURSON_TYPES_HPP
#define PURSON_TYPES_HPP 1

/**
 * Constructs describing the type system of purson
 * 
 * Purson is statically typed, but types are usually inferred.
 **/

namespace purson{
	//! base type of all types
	//! programming tool and implementation detail
	class type{
		public:
			virtual ~type() = default;
	};
	
	//! base for void types (probably the only)
	class void_type: public type{};

	//! base for boolean types
	class boolean_type: public type{};

	//! base for numeric types
	class numeric_type: public type{};

	//! base for character types
	class character_type: public type{};

	//! base for string types
	class string_type: public type{};
}

#endif // !PURSON_TYPES_HPP
