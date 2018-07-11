#ifndef PURSON_TYPES_HPP
#define PURSON_TYPES_HPP 1

#include <memory>
#include <vector>

/**
 * @headerfile types.hpp
 * 
 * Constructs describing the type system of purson
 * 
 * Purson is statically typed, but types are usually inferred.
 **/

#include "exception.hpp"
#include "types/numeric.hpp"

namespace purson{
	class type_error: public exception{ using exception::exception; };
	
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
			 * Get integer type
			 * 
			 * @param[in] bits number of bits in integer
			 * @param[in] signed_ whether the integer is signed
			 * @returns nullptr if type not found, otherwise the integer type
			 **/
			virtual const integer_type *integer(std::uint32_t bits, bool signed_ = true) const = 0;
			
			/**
			 * Get real type
			 * 
			 * @param[in] bits number of bits in real
			 * @param[in] ieee754 whether the real is a ieee754 float
			 * @returns nullptr if type not found, otherwise the real type
			 **/
			virtual const real_type *real(std::uint32_t bits, bool ieee754 = true) const = 0;
			
			/**
			 * Get function type
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
