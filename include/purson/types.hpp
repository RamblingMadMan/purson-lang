#ifndef PURSON_TYPES_HPP
#define PURSON_TYPES_HPP 1

#include <memory>
#include <vector>

/**
 * @headerfile types.hpp
 * 
 * Constructs describing the type system of purson
 **/

#include "expressions/base.hpp"

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
			 * Get unit type
			 * 
			 * @returns the unit type
			 **/
			virtual const unit_type *unit() const noexcept = 0;

			/**
			 * Get type value type
			 *
			 * @returns the type value type
			 */
		 	virtual const type_type *type_() const noexcept = 0;

		 	/**
		 	 * Get string type
		 	 *
		 	 * @param[in] encoding the string encoding for the type
		 	 * @returns nullptr if type not found, otherwise the string type
		 	 */
		 	virtual const string_type *string(char_encoding encoding) const noexcept = 0;

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

	// annoying class declaration needed for function declaration
	class type_block_expr;

	/**
	 * Return a type that fulfils the requirements set in the block
	 *
	 * @param[in] block the type block
	 * @param[in] types typeset to use
	 * @returns the solved return type
	 */
 	const type *solve_type(const type_block_expr *block, const typeset *types);
	
	/**
	 * Get an immutable set of all basic types
	 * 
	 * @param[in] ver version string
	 **/
	const typeset *types(std::string_view ver);
	
	/**
	 * Get a mutable set of types initialized with every basic type
	 * 
	 * @param[in] ver version string
	 **/
	std::unique_ptr<typeset> mutable_types(std::string_view ver);

	namespace detail{
		template<typename...>
		class get_type_helper;

		template<typename Arg>
		void push_arg_types(const typeset *ts, std::vector<const type *> &tys){
			tys.push_back(get_type_helper<Arg>::get(ts));
		}

		template<typename Arg, typename Arg0, typename ... Args>
		void push_arg_types(const typeset *ts, std::vector<const type *> &tys){
			push_arg_types<Arg>(ts, tys);
			push_arg_types<Arg0, Args...>(ts, tys);
		}

		template<>
		class get_type_helper<void>{
			public:
				static auto get(const typeset *ts) noexcept{ return ts->unit(); }
		};

		template<>
		class get_type_helper<std::int16_t>{
			public:
				static auto get(const typeset *ts) noexcept{ return ts->integer(16); }
		};

		template<>
		class get_type_helper<std::int32_t>{
			public:
				static auto get(const typeset *ts) noexcept{ return ts->integer(32); }
		};

		template<>
		class get_type_helper<std::int64_t>{
			public:
				static auto get(const typeset *ts) noexcept{ return ts->integer(64); }
		};

		template<>
		class get_type_helper<std::uint16_t>{
			public:
				static auto get(const typeset *ts) noexcept{ return ts->natural(16); }
		};

		template<>
		class get_type_helper<std::uint32_t>{
			public:
				static auto get(const typeset *ts) noexcept{ return ts->natural(32); }
		};

		template<>
		class get_type_helper<std::uint64_t>{
			public:
				static auto get(const typeset *ts) noexcept{ return ts->natural(64); }
		};

		template<typename Ret, typename ... Params>
		class get_type_helper<Ret(Params...)>{
			public:
				static auto get(const typeset *ts) noexcept{
					std::vector<const type*> param_tys;
					param_tys.reserve(sizeof...(Params));
					detail::push_arg_types<Params...>(ts, param_tys);
					return ts->function(get_type_helper<Ret>::get(ts), param_tys);
				}
		};
	}

	template<typename T>
	auto get_type(const typeset *ts){ return detail::get_type_helper<T>::get(ts); }
}

#endif // !PURSON_TYPES_HPP
