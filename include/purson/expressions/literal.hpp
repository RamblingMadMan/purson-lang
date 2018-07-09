#ifndef PURSON_EXPRESSIONS_LITERAL_HPP
#define PURSON_EXPRESSIONS_LITERAL_HPP 1

#include <gmp.h>
#include <mpfr.h>

#include "base.hpp"

namespace purson{
	//! base for all literal expressions
	class literal_expr: public rvalue_expr{};
	
	//! base for all numeric literal expressions
	class numeric_literal_expr: public literal_expr{};
	
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
			integer_literal_expr(std::string_view lit, const typeset *types){
				detail::init_set_mpz(m_val, lit);
				if(mpz_fits_sint_p(m_val))
					m_type = types->integer(32);
				else if(mpz_fits_slong_p(m_val))
					m_type = types->integer(64);
				else if(mpz_fits_ulong_p(m_val))
					m_type = types->integer(64, false);
				else
					m_type = types->integer(64);
			}
			
			~integer_literal_expr(){
				mpz_clear(m_val);
			}
			
			const integer_type *value_type() const noexcept override{ return m_type; }
			
		private:
			mpz_t m_val;
			const integer_type *m_type;
	};
	
	class fraction_literal_expr: public numeric_literal_expr{
		public:
			fraction_literal_expr(std::string_view num, std::string_view denom, const typeset *types){
				mpq_init(m_val);
				
				mpz_t mpz_num, mpz_denom;
				detail::init_set_mpz(mpz_num, num);
				detail::init_set_mpz(mpz_denom, denom);
				
				mpq_set_num(m_val, mpz_num);
				mpq_set_den(m_val, mpz_denom);
				
				mpq_canonicalize(m_val);
				
				mpz_clears(mpz_num, mpz_denom);
			}
			
			~fraction_literal_expr(){
				mpq_clear(m_val);
			}
			
		private:
			mpq_t m_val;
			const integer_type *m_type;
	};
	
	class real_literal_expr: public numeric_literal_expr{
		public:
			real_literal_expr(std::string_view lit, const typeset *types){
				mpfr_init_set_str(m_val, lit.data(), 10, MPFR_RNDN);
				m_type = types->real(32);
			}
			
			~real_literal_expr(){
				mpfr_clear(m_val);
			}
			
			const real_type *value_type() const noexcept override{ return m_type; }
			
		private:
			mpfr_t m_val;
			const real_type *m_type;
	};
}

#endif // !PURSON_EXPRESSIONS_LITERAL_HPP
