#include <cmath>
#include <map>

#include "fmt/format.h"

#include "purson/types.hpp"

namespace purson{
	struct basic_type: virtual type{
		basic_type(std::size_t bits_, std::string_view id)
			: m_bits(bits_), m_str(fmt::format("{}{}", id, bits_)){}
		
		std::size_t bits() const noexcept override{ return m_bits; }
		std::string_view str() const noexcept override{ return m_str; }
			
		std::size_t m_bits;
		std::string m_str;
	};
	
	struct basic_integer: basic_type, integer_type{
		basic_integer(std::size_t bits_, bool signed_)
			: basic_type(bits_, signed_ ? "i" : "u"), m_is_signed(signed_){}
		
		bool is_signed() const noexcept override{ return m_is_signed; }
		
		bool m_is_signed;
	};
	
	struct basic_real: basic_type, real_type{
		basic_real(std::size_t bits_, bool ieee754)
			: basic_type(bits_, "r"), m_is_ieee754(ieee754){}
		
		bool is_ieee754() const noexcept override{ return m_is_ieee754; }
			
		bool m_is_ieee754;
	};
	
	class base_typeset: public typeset{
		public:
			const type *get(std::string_view name) const noexcept override{
				switch(name[0]){
					case 'u':
					case 'i':{
						bool signed_ = (name[0] != 'u');
						std::size_t substr_beg = (signed_ ? 0 : 1);
						
						auto substr = name.substr(substr_beg);
						
						if(substr.substr(0, 3) == "int"){
							if((substr == "int") || (substr == "int32")) return &m_int_types[(substr_beg * 4) + 2];
							else if(substr == "int64") return &m_int_types[(substr_beg * 4) + 3];
							else if(substr == "int16") return &m_int_types[(substr_beg * 4) + 1];
							else if(substr == "int8") return &m_int_types[(substr_beg * 4)];
						}
						
						break;
					}
					
					case 'r':{
						if(name.substr(0, 4) == "real"){
							if((name == "real") || (name == "real32")) return &m_real_types[0];
							else if(name == "real64") return &m_real_types[1];
						}
						
						break;
					}
					
					default: break;
				}
				
				return nullptr;
			}
			
			const integer_type *integer(std::uint32_t bits, bool is_signed) const noexcept override{
				switch(bits){
					case 8:
					case 16:
					case 32:
					case 64:{
						auto idx = (int)std::log2(bits) - 3;
						return &m_int_types[(!is_signed * 4) + idx];
					}
					
					default: return nullptr;
				}
			}
			
			const real_type *real(std::uint32_t bits, bool ieee754) const noexcept override{
				switch(bits){
					case 8:
					case 16:
					case 32:
					case 64:
						return &m_real_types[(int)std::log2(bits) - 3];
					
					default: return nullptr;
				}
			}
			
		private:
			basic_integer m_int_types[8]{
				{8, true}, {16, true}, {32, true}, {64, true},
				{8, false}, {16, false}, {32, false}, {64, false}
			};
			
			basic_real m_real_types[2]{
				{32, true}, {64, true}
			};
	};
	
	static base_typeset purson_base_types;
	
	const typeset *types(std::string_view ver){
		return &purson_base_types;
	}
}
