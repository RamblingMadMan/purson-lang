#ifndef PURSON_LIB_PARSER_HPP
#define PURSON_LIB_PARSER_HPP 1

#include <memory>
#include <map>

#include "purson/parser.hpp"
#include "purson/types.hpp"
#include "purson/expressions.hpp"

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
			
			std::shared_ptr<lvalue_expr> get_var(std::string_view name) const noexcept{
				if(auto res = m_vars.find(name); res != end(m_vars))
					return res->second;
				else if(m_parent)
					return m_parent->get_var(name);
				else
					return nullptr;
			}
			
			void set_var(std::string_view name, std::shared_ptr<lvalue_expr> var){
				m_vars[name] = var;
			}
			
			std::shared_ptr<fn_expr> get_fn(std::string_view mangled_name) const noexcept{
				if(auto res = m_fns.find(mangled_name); res != end(m_fns))
					return res->second;
				else if(m_parent)
					return m_parent->get_fn(mangled_name);
				else
					return nullptr;
			}
			
			void set_fn(std::string_view mangled_name, std::shared_ptr<fn_expr> fn){
				m_fns[mangled_name] = fn;
			}
			
			const class typeset *typeset() const noexcept{ return m_types; }
		
		private:
			const parser_scope *m_parent;
			const class typeset *m_types;
			
			std::map<std::string_view, const type*> m_type_map;
			std::map<std::string_view, std::shared_ptr<fn_expr>> m_fns;
			std::map<std::string_view, std::shared_ptr<lvalue_expr>> m_vars;
	};
	
	using token_iterator_t = typename std::vector<token>::const_iterator;
	
	inline bool default_delim(const token &tok){ return tok.type() == token_type::end; }
	
	using delim_fn_t = std::function<bool(const token&)>;
	
	std::shared_ptr<expr> parse_top(token_iterator_t &it, token_iterator_t end, parser_scope &scope);
	std::shared_ptr<expr> parse_inner(delim_fn_t delim_fn, token_iterator_t &it, token_iterator_t end, parser_scope &scope);
	std::shared_ptr<rvalue_expr> parse_value(delim_fn_t delim_fn, token_iterator_t &it, token_iterator_t end, parser_scope &scope);
	
	std::shared_ptr<rvalue_expr> parse_literal(const token &lit, delim_fn_t delim_fn, token_iterator_t &it, token_iterator_t end, parser_scope &scope);
	std::shared_ptr<rvalue_expr> parse_unary_op(const token &op, delim_fn_t delim_fn, token_iterator_t &it, token_iterator_t end, parser_scope &scope);
	std::shared_ptr<rvalue_expr> parse_binary_op(std::shared_ptr<rvalue_expr> lhs, const token &op, delim_fn_t delim_fn, token_iterator_t &it, token_iterator_t end, parser_scope &scope);
	
	std::shared_ptr<expr> parse_keyword(const token &kw, delim_fn_t delim_fn, token_iterator_t &it, token_iterator_t end, parser_scope &scope);
	
	std::shared_ptr<rvalue_expr> parse_id(const token &id, delim_fn_t delim_fn, token_iterator_t &it, token_iterator_t end, parser_scope &scope);
	std::shared_ptr<rvalue_expr> parse_fn(const token &fn, delim_fn_t delim_fn, token_iterator_t &it, token_iterator_t end, parser_scope &scope);
	std::shared_ptr<lvalue_expr> parse_var(const token &var, delim_fn_t delim_fn, token_iterator_t &it, token_iterator_t end, parser_scope &scope);
}

#endif // !PURSON_LIB_PARSER_HPP
