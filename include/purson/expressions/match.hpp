#ifndef PURSON_EXPRESSIONS_MATCH_HPP
#define PURSON_EXPRESSIONS_MATCH_HPP 1

#include <vector>
#include <memory>

#include "base.hpp"

namespace purson{
	class match_expr: public rvalue_expr{
		public:
			match_expr(
				std::shared_ptr<const rvalue_expr> checked_,
				const std::vector<std::pair<std::shared_ptr<const rvalue_expr>, std::shared_ptr<const rvalue_expr>>> &patterns_
			): m_checked{checked_}, m_patterns{patterns_}{
				if(patterns_.size() < 1)
					throw expr_error{"match expression expects at least 1 pattern"};
				
				const type *common_type = patterns_[0].second->value_type();
				for(std::size_t i = 1; i < patterns_.size(); i++)
					common_type = promote_type(common_type, patterns_[i].second->value_type());
				
				m_value_type = common_type;
			}
			
			const type *value_type() const noexcept override{ return m_value_type; }
			
			const std::shared_ptr<const rvalue_expr> &checked() const noexcept{ return m_checked; }
			
			const std::vector<std::pair<std::shared_ptr<const rvalue_expr>, std::shared_ptr<const rvalue_expr>>> patterns() const noexcept{
				return m_patterns;
			}
			
		private:
			std::shared_ptr<const rvalue_expr> m_checked;
			std::vector<std::pair<std::shared_ptr<const rvalue_expr>, std::shared_ptr<const rvalue_expr>>> m_patterns;
			const type *m_value_type;
	};
}

#endif // !PURSON_EXPRESSIONS_MATCH_HPP
