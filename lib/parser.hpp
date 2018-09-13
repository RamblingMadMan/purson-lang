#ifndef PURSON_LIB_PARSER_HPP
#define PURSON_LIB_PARSER_HPP 1

#include <memory>
#include <map>

#include "purson/parser.hpp"
#include "purson/types.hpp"
#include "purson/expressions.hpp"

/**
 * 
 * @file lib/parser.hpp
 * 
 * 
 **/

namespace purson{
	struct parser_scope{
		public:
			parser_scope(const class typeset *types_, const parser_scope *parent_ = nullptr)
				: m_parent(parent_), m_types(types_){}
			
			const type *get_type(std::string_view name) const{
				if(auto res = m_type_map.find(name); res != end(m_type_map))
					return res->second;
				else if(auto ty = m_types->get(name))
					return ty;
				else if(m_parent)
					return m_parent->get_type(name);
				else
					return nullptr;
			}
			
			void set_type(std::string_view name, const type *ty) noexcept{
				m_type_map[name] = ty;
			}
			
			std::shared_ptr<const var_decl_expr> get_var(std::string_view name) const noexcept{
				if(auto res = m_vars.find(name); res != end(m_vars))
					return res->second;
				else if(m_parent)
					return m_parent->get_var(name);
				else
					return nullptr;
			}
			
			void set_var(std::string_view name, std::shared_ptr<const var_decl_expr> var){
				m_vars[name] = var;
			}
			
			std::vector<std::shared_ptr<const fn_expr>> get_fn(std::string_view name) const noexcept{
				if(auto res = m_fns.find(name); res != end(m_fns)){
					auto &&subs_map = res->second;
					std::vector<std::shared_ptr<const fn_expr>> ret;
					ret.reserve(subs_map.size());
					for(auto &&sub : subs_map)
						ret.push_back(sub.second);
					
					return ret;
				}
				else if(m_parent)
					return m_parent->get_fn(name);
				else
					return {};
			}
			
			void add_fn(std::string_view name, const std::vector<const type*> &subs, std::shared_ptr<const fn_expr> fn){
				m_fns[name][subs] = std::move(fn);
			}
			
			const class typeset *typeset() const noexcept{ return m_types; }
		
		private:
			const parser_scope *m_parent;
			const class typeset *m_types;
			
			std::map<std::string_view, const type*> m_type_map;
			std::map<std::string_view, std::map<std::vector<const type*>, std::shared_ptr<const fn_expr>>> m_fns;
			std::map<std::string_view, std::shared_ptr<const var_decl_expr>> m_vars;
	};
	
	using token_iterator_t = typename std::vector<token>::const_iterator;
	
	inline bool default_delim(const token &tok){ return tok.type() == token_type::end; }
	
	using delim_fn_t = std::function<bool(const token&)>;
	
	std::shared_ptr<const expr> parse_top(token_iterator_t &it, token_iterator_t end, parser_scope &scope);
	std::shared_ptr<const rvalue_expr> parse_inner(delim_fn_t delim_fn, token_iterator_t &it, token_iterator_t end, parser_scope &scope);
	std::shared_ptr<const rvalue_expr> parse_value(delim_fn_t delim_fn, token_iterator_t &it, token_iterator_t end, parser_scope &scope);
	std::shared_ptr<const rvalue_expr> parse_leading_value(std::shared_ptr<const rvalue_expr> val, delim_fn_t delim_fn, token_iterator_t &it, token_iterator_t end, parser_scope &scope);
	
	std::shared_ptr<const rvalue_expr> parse_literal(const token &lit, delim_fn_t delim_fn, token_iterator_t &it, token_iterator_t end, parser_scope &scope);
	std::shared_ptr<const rvalue_expr> parse_unary_op(const token &op, delim_fn_t delim_fn, token_iterator_t &it, token_iterator_t end, parser_scope &scope);
	std::shared_ptr<const rvalue_expr> parse_binary_op(std::shared_ptr<const rvalue_expr> lhs, const token &op, delim_fn_t delim_fn, token_iterator_t &it, token_iterator_t end, parser_scope &scope);
	
	std::shared_ptr<const rvalue_expr> parse_keyword(const token &kw, delim_fn_t delim_fn, token_iterator_t &it, token_iterator_t end, parser_scope &scope);
	
	std::shared_ptr<const rvalue_expr> parse_id(const token &id, delim_fn_t delim_fn, token_iterator_t &it, token_iterator_t end, parser_scope &scope);
	std::shared_ptr<const rvalue_expr> parse_fn(const token &fn, fn_visibility visibility, fn_linkage linkage, delim_fn_t delim_fn, token_iterator_t &it, token_iterator_t end, parser_scope &scope);
	std::shared_ptr<const lvalue_expr> parse_var(const token &var, delim_fn_t delim_fn, token_iterator_t &it, token_iterator_t end, parser_scope &scope);
	std::shared_ptr<const lvalue_expr> parse_type(const token &type, delim_fn_t delim_fn, token_iterator_t &it, token_iterator_t end, parser_scope &scope);
	std::shared_ptr<const rvalue_expr> parse_match(const token &match, delim_fn_t delim_fn, token_iterator_t &it, token_iterator_t end, parser_scope &scope);
}

#endif // !PURSON_LIB_PARSER_HPP
