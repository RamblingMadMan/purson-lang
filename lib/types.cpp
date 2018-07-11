#include <cmath>
#include <map>
#include <list>

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
		
		protected:
			void set_str(std::string_view str_){ m_str = str_; }
			void set_str(std::string &&str_){ m_str = std::move(str_); }
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
	
	struct basic_function: basic_type, function_type{
		basic_function(std::size_t bits_, const type *return_type_, const std::vector<const type*> &param_types_)
			: basic_type(bits_, ""), m_return_type{return_type_}, m_param_types{param_types_}{
			std::string tmp_str = fmt::format("f{}", param_types_.size());
			
			for(std::size_t i = 0; i < param_types_.size(); i++){
				if(!param_types_[i]) throw type_error{fmt::format("null type for parameter {}", i + 1)};
				tmp_str = fmt::format("{}{}", tmp_str, param_types_[i]->str());
			}
			
			if(!return_type_) throw type_error{"null return type given for function type"};
			tmp_str = fmt::format("{}{}", tmp_str, return_type_->str());
			set_str(std::move(tmp_str));
		}
		
		const type *return_type() const noexcept override{ return m_return_type; }
		const std::size_t num_params() const noexcept override{ return m_param_types.size(); }
		const type *param_type(std::size_t idx) const noexcept override{ return idx < m_param_types.size() ? m_param_types[idx] : nullptr; }
			
		const type *m_return_type;
		std::vector<const type*> m_param_types;
	};
	
	class base_typeset: public typeset{
		public:
			const type *get(std::string_view name) const override{
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
			
			const integer_type *integer(std::uint32_t bits, bool is_signed) const override{
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
			
			const real_type *real(std::uint32_t bits, bool ieee754) const override{
				if(!ieee754) return nullptr;
				
				switch(bits){
					case 8:
					case 16:
					case 32:
					case 64:
						return &m_real_types[(int)std::log2(bits) - 3];
					
					default: return nullptr;
				}
			}
			
			const function_type *function(const type *return_type, const std::vector<const type*> &param_types) const override{
				auto ret_ty_res = m_fn_types.find(return_type);
				if(ret_ty_res == end(m_fn_types)){
					auto res = m_fn_types.emplace(return_type, std::map<std::size_t, std::list<basic_function>>{});
					if(!res.second)
						throw type_error{"could not create function type map for return type"};
				
					ret_ty_res = res.first;
				}
				
				auto num_params_res = ret_ty_res->second.find(param_types.size());
				if(num_params_res == end(ret_ty_res->second)){
					auto res = ret_ty_res->second.emplace(param_types.size(), std::list<basic_function>{});
					if(!res.second)
						throw type_error{"could not create function type map for number of parameters"};
					
					num_params_res = res.first;
				}
				
				for(auto &&fn_ty : num_params_res->second){
					bool good = true;
					for(std::size_t i = 0; i < fn_ty.num_params(); i++){
						if(fn_ty.param_type(i) != param_types[i]){
							good = false;
							break;
						}
					}
					
					if(good) return &fn_ty;
				}
				
				return &num_params_res->second.emplace_back(64, return_type, param_types);
			}
			
		private:
			basic_integer m_int_types[8]{
				{8, true}, {16, true}, {32, true}, {64, true},
				{8, false}, {16, false}, {32, false}, {64, false}
			};
			
			basic_real m_real_types[2]{
				{32, true}, {64, true}
			};
			
			mutable std::map<const type*, std::map<std::size_t, std::list<basic_function>>> m_fn_types;
	};
	
	static base_typeset purson_base_types;
	
	const typeset *types(std::string_view ver){
		return &purson_base_types;
	}
}
