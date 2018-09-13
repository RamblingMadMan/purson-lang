#include <cmath>
#include <map>
#include <list>

#include "fmt/format.h"

#include "purson/expressions/type.hpp"
#include "purson/types.hpp"

namespace purson{
	const type *promote_type(const type *a, const type *b){
		using real_typep_t = const real_type*;
		using rational_typep_t = const rational_type*;
		using integer_typep_t = const integer_type*;
		using natural_typep_t = const natural_type*;
		
		auto greater_of = [](auto lhs, auto rhs){
			return lhs->bits() > rhs->bits() ? lhs : rhs;
		};
		
		if(!a && !b) return nullptr;
		else if(!a) return b;
		else if(!b) return a;
		else if(a == b) return a;
		else if(dynamic_cast<const arithmetic_type*>(a) && dynamic_cast<const arithmetic_type*>(b)){
			real_typep_t ar = dynamic_cast<real_typep_t>(a), br = dynamic_cast<real_typep_t>(b);
			rational_typep_t aq = dynamic_cast<rational_typep_t>(a), bq = dynamic_cast<rational_typep_t>(b);
			integer_typep_t ai = dynamic_cast<integer_typep_t>(a), bi = dynamic_cast<integer_typep_t>(b);
			natural_typep_t an = dynamic_cast<natural_typep_t>(a), bn = dynamic_cast<natural_typep_t>(b);
			
			if(an || bn){
				if(an && bn)
					return greater_of(an, bn);
				else
					return an ? b : an;
			}
			else if(ai || bi){
				if(ai && bi)
					return greater_of(ai, bi);
				else
					return ai ? b : ai;
			}
			else if(aq || bq){
				if(aq && bq)
					return greater_of(aq, bq);
				else
					return aq ? b : aq;
			}
			else if(ar || br){
				if(ar && br)
					return greater_of(ar, br);
				else
					return ar ? b : ar;
			}
			else
				throw type_error{"unknown arithmetic_type, can't promote either side :^("};
		}
		else
			throw type_error{"only arithmetic types can be promoted currently"};
	}
	
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
	
	struct basic_unit: basic_type, unit_type{
		basic_unit()
			: basic_type(0, "u"){}
	};

	struct basic_type_type: basic_type, type_type{
		basic_type_type()
			: basic_type(0, "t"){}
	};

	struct basic_string: basic_type, string_type{
		basic_string(char_encoding encoding_)
			: basic_type(8, "s"), m_encoding(encoding_){}

		char_encoding encoding() const noexcept override{ return m_encoding; }

		char_encoding m_encoding;
	};

	struct basic_boolean: basic_type, boolean_type{
		basic_boolean(std::size_t bits_)
			: basic_type(bits_, "b"){}
	};
	
	struct basic_natural: basic_type, natural_type{
		basic_natural(std::size_t bits_)
			: basic_type(bits_, "n"){}
	};
	
	struct basic_integer: basic_type, integer_type{
		basic_integer(std::size_t bits_)
			: basic_type(bits_, "i"){}
	};
	
	struct basic_rational: basic_type, rational_type{
		basic_rational(std::size_t bits_)
			: basic_type(bits_, "q"){}
	};
	
	struct basic_real: basic_type, real_type{
		basic_real(std::size_t bits_, bool ieee754)
			: basic_type(bits_, "r"), is_ieee754(ieee754){}
		
		const bool is_ieee754;
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
					case 'I':{
						if(name.substr(0, 7) == "Integer"){
							if((name == "Integer") || (name == "Integer32")) return &m_integer_types[2];
							else if(name == "Integer64") return &m_integer_types[3];
							else if(name == "Integer16") return &m_integer_types[1];
							else if(name == "Integer8") return &m_integer_types[0];
						}
						
						break;
					}
					
					case 'R':{
						if(name.substr(0, 4) == "Real"){
							if((name == "Real") || (name == "Real32")) return &m_real_types[0];
							else if(name == "Real64") return &m_real_types[1];
						}
						
						break;
					}

					case 'T':{
						if(name == "Type") return type_();
						break;
					}

					case 'U':{
						if(name == "Unit") return unit();
						break;
					}
					
					default: break;
				}
				
				return nullptr;
			}
			
			const unit_type *unit() const noexcept override{
				return &m_unit_type;
			}

			const type_type *type_() const noexcept override{
				return &m_type_type;
			}

			const string_type *string(char_encoding encoding) const noexcept override{
				switch(encoding){
					case char_encoding::ascii: return &m_string_types[0];
					case char_encoding::utf8: return &m_string_types[1];
					case char_encoding::utf16: return &m_string_types[2];
					case char_encoding::utf32: return &m_string_types[3];
					default: return nullptr;
				}
			}
			
			const natural_type *natural(std::uint32_t bits) const override{
				switch(bits){
					case 8:
					case 16:
					case 32:
					case 64:
						return &m_natural_types[(int)std::log2(bits) - 3];
					
					default: return nullptr;
				}
			}
			
			const integer_type *integer(std::uint32_t bits) const override{
				switch(bits){
					case 8:
					case 16:
					case 32:
					case 64:
						return &m_integer_types[(int)std::log2(bits) - 3];
					
					default: return nullptr;
				}
			}
			
			const rational_type *rational(const integer_type *integer_type_) const override{
				switch(integer_type_->bits()){
					case 8:
					case 16:
					case 32:
					case 64:
						return &m_rational_types[(int)std::log2(integer_type_->bits()) - 3];
					
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
			basic_unit m_unit_type;
			basic_type_type m_type_type;

			basic_string m_string_types[4]{
				{char_encoding::ascii}, {char_encoding::utf8}, {char_encoding::utf16}, {char_encoding::utf32}
			};
			
			basic_natural m_natural_types[4]{
				{8}, {16}, {32}, {64}
			};
			
			basic_integer m_integer_types[4]{
				{8}, {16}, {32}, {64}
			};
			
			basic_rational m_rational_types[4]{
				{16}, {32}, {64}, {128}
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

	struct basic_type_type_base_t{ virtual ~basic_type_type_base_t() = default; };

	template<typename T, typename Enable = void>
	struct basic_type_t;

	template<typename T>
	struct basic_type_t<T, std::enable_if_t<std::is_base_of_v<basic_type, T>>>
		: basic_type_type_base_t{};

	template<typename T>
	const basic_type_type_base_t *type_type(){
		static basic_type_t<T> ret;
		return &ret;
	}

	const type *solve_type(const type_block_expr *block, const typeset *types){
		std::vector<const type*> members;

		auto ty = type_type<basic_unit>();
		for(auto &&expr : block->exprs()){

		}
		throw type_error{"type axiom solving not currently implemented"};
	}
}
