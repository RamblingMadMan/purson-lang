#ifndef PURSON_TYPES_HPP
#define PURSON_TYPES_HPP 1

#include <memory>
#include <vector>

/**
 * @headerfile types.hpp
 * 
 * Constructs describing the type system of purson
 **/

#include "types/numeric.hpp"

namespace purson{
	/**
	 * Set of types within the language
	 **/
	class typeset{
		public:
			/**
			 * Get a type by name
			 * 
			 * @param[in] name type name
			 * @returns nullptr if no type was found, otherwise the type
			 **/
			virtual const type *get(std::string_view name) const = 0;
			
			/**
			 * Get natural type
			 * 
			 * @param[in] bits number of bits in underlying type
			 * @returns nullptr if type not found, otherwise the natural type
			 **/
			virtual const natural_type *natural(std::uint32_t bits) const = 0;
			
			/**
			 * Get integer type
			 * 
			 * @param[in] bits number of bits in underlying type
			 * @returns nullptr if type not found, otherwise the integer type
			 **/
			virtual const integer_type *integer(std::uint32_t bits) const = 0;
			
			/**
			 * Get rational type
			 * 
			 * @returns nullptr if type not found, otherwise the rational type
			 **/
			virtual const rational_type *rational(const integer_type *integer_type_) const = 0;
			
			/**
			 * Get real type
			 * 
			 * @param[in] bits number of bits in underlying type
			 * @param[in] ieee754 whether the underlying type is a ieee754 float
			 * @returns nullptr if type not found, otherwise the real type
			 **/
			virtual const real_type *real(std::uint32_t bits, bool ieee754 = true) const = 0;
			
			/**
			 * Get function type
			 * 
			 * @param[in] return_type type for return value
			 * @param[in] param_types types for the parameters
			 * @returns nullptr if type not found, otherwise the function type
			 **/
			virtual const function_type *function(const type *return_type, const std::vector<const type*> &param_types) const = 0;
	};
	
	/**
	 * Get an immutable set of all basic types
	 * 
	 * @param[in] ver version string
	 **/
	const typeset *types(std::string_view ver);
	
	/**
	 * Get a mutable set of types initialized with every basic types
	 * 
	 * @param[in] ver version string
	 **/
	std::unique_ptr<typeset> mutable_types(std::string_view ver);
}

#endif // !PURSON_TYPES_HPP
