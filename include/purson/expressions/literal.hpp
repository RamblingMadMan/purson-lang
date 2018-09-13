#ifndef PURSON_EXPRESSIONS_LITERAL_HPP
#define PURSON_EXPRESSIONS_LITERAL_HPP 1

#include <cmath>

#include <gmp.h>
#include <mpfr.h>

#include "base.hpp"
#include "../types.hpp"

#include "utf8.h"

namespace purson{
	//! base for all literal expressions
	class literal_expr: public rvalue_expr{};

	class string_literal_expr: public literal_expr{
		public:
			string_literal_expr(std::string_view lit, const typeset *types){
				auto it = begin(lit);
				auto end_ = end(lit);

				auto cp = utf8::next(it, end_);

				if(cp != '"')
					throw expr_error{"invalid string literal"};

				while(it != end_){
					cp = utf8::next(it, end_);
					if(cp == '\\'){
						cp = utf8::next(it, end_);

						char c;

						switch(cp){
							case 'n': c = '\n'; break;
							case 'r': c = '\r'; break;
							case '"': c = '"'; break;
							case '\'': c = '\''; break;

							default:
								throw expr_error{"invalid string escape character"};
						}

						m_str += c;
					}
					else if(cp == '"'){
						if(it != end_)
							throw expr_error{"string literal has unescaped quote mark"};

						break;
					}
					else
						utf8::append(cp, std::back_inserter(m_str));
				}

				m_type = types->string(char_encoding::utf8);
			}

			std::string_view str() const noexcept override{ return m_str; }

			const string_type *value_type() const noexcept override{ return m_type; };

		private:
			std::string m_str;
			const string_type *m_type;
	};
	
	//! base for all numeric literal expressions
	class numeric_literal_expr: public literal_expr{
		public:
			numeric_literal_expr(std::string_view lit): m_str(lit){}

			std::string_view str() const noexcept override{ return m_str; }

		protected:
			std::string m_str;
	};
	
	namespace detail{
		inline void init_set_mpz(mpz_t val, std::string_view lit){
			if(lit[1] == 'x')
				mpz_init_set_str(val, lit.data(), 16);
			else if(lit[1] == 'b')
				mpz_init_set_str(val, lit.data(), 2);
			else
				mpz_init_set_str(val, lit.data(), 10);
		}
	}
	
	class integer_literal_expr: public numeric_literal_expr{
		public:
			integer_literal_expr(std::string_view lit, const typeset *types): numeric_literal_expr(lit){
				detail::init_set_mpz(m_val, lit);
				if(mpz_fits_sint_p(m_val))
					m_type = types->integer(32);
				else if(mpz_fits_slong_p(m_val))
					m_type = types->integer(64);
				else
					throw expr_error{"integer literal is too large to fit in any underlying type"};
			}
			
			~integer_literal_expr(){
				mpz_clear(m_val);
			}
			
			const mpz_t &value() const noexcept{ return m_val; }
			
			const integer_type *value_type() const noexcept override{ return m_type; }

			std::string_view str() const noexcept override{ return m_str; }
			
		private:
			mpz_t m_val;
			const integer_type *m_type;
	};
	
	class natural_literal_expr: public numeric_literal_expr{
		public:
			natural_literal_expr(std::string_view lit, const typeset *types): numeric_literal_expr(lit){
				detail::init_set_mpz(m_val, lit);
				if(mpz_fits_uint_p(m_val))
					m_type = types->natural(32);
				else if(mpz_fits_ulong_p(m_val))
					m_type = types->natural(64);
				else
					throw expr_error{"natural literal is too large to fit in any underlying type"};
			}
			
			~natural_literal_expr(){
				mpz_clear(m_val);
			}
			
			const mpz_t &value() const noexcept{ return m_val; }
			
			const natural_type *value_type() const noexcept override{ return m_type; }
			
		private:
			mpz_t m_val;
			const natural_type *m_type;
	};
	
	class rational_literal_expr: public numeric_literal_expr{
		public:
			rational_literal_expr(std::string_view num, std::string_view denom, const typeset *types)
				: numeric_literal_expr(std::string(num) + '/' + std::string(denom)){
				mpq_init(m_val);
				
				mpz_t mpz_num, mpz_denom;
				detail::init_set_mpz(mpz_num, num);
				detail::init_set_mpz(mpz_denom, denom);
				
				mpq_set_num(m_val, mpz_num);
				mpq_set_den(m_val, mpz_denom);
				
				std::size_t bits = std::pow(2, std::ceil(std::log2(std::max(mpz_sizeinbase(mpz_num, 2), mpz_sizeinbase(mpz_denom, 2)))));
				
				switch(bits){
					case 8: 
					case 16:
					case 32:
					case 64: break;
					default:
						throw expr_error{"rational literal is too large to fit in any underlying type"};
				}
				
				m_type = types->rational(types->integer(bits));
				
				mpq_canonicalize(m_val);
				
				mpz_clears(mpz_num, mpz_denom);
			}
			
			~rational_literal_expr(){
				mpq_clear(m_val);
 			}
 			
 			const mpq_t &value() const noexcept{ return m_val; }
 			
 			const rational_type *value_type() const noexcept{ return m_type; }
			
		private:
			mpq_t m_val;
			const rational_type *m_type;
	};
	
	class real_literal_expr: public numeric_literal_expr{
		public:
			real_literal_expr(std::string_view lit, const typeset *types)
				: numeric_literal_expr(lit){
				mpfr_init_set_str(m_val, lit.data(), 10, MPFR_RNDN);
				m_type = types->real(32);
			}
			
			~real_literal_expr(){
				mpfr_clear(m_val);
			}
			
			const mpfr_t &value() const noexcept{ return m_val; }
			
			const real_type *value_type() const noexcept override{ return m_type; }
			
		private:
			mpfr_t m_val;
			const real_type *m_type;
	};
}

#endif // !PURSON_EXPRESSIONS_LITERAL_HPP
