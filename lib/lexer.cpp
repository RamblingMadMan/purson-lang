#include "purson/lexer.hpp"
#include "purson/operator.hpp"

#include <unicode/uchar.h>

#include "utf8.h"
#include "fmt/format.h"

namespace purson{
	struct cp_data_t{
		std::uint32_t cp;
		std::size_t line, col;
	};
	
	bool is_keyword(std::string_view kw){
		return
			(kw == "var") ||
			(kw == "fn") ||
			(kw == "type") ||
			(kw == "axiom") ||
			(kw == "match") ||
			(kw == "if") ||
			(kw == "else");
	}
	
	bool is_bracket(std::uint32_t cp){
		switch(cp){
			case '(':
			case ')':
			case '{':
			case '}':
			case '[':
			case ']':
				return true;
				
			default: return false;
		}
	}
	
	std::vector<token> lex(std::string_view ver, std::string_view name, std::string_view src){
		std::vector<token> ret;
		
		auto it = begin(src);
		auto it_end = end(src);
		
		auto next_cp = [&it, it_end, line = (std::size_t)1, col = (std::size_t)0]() mutable -> cp_data_t{
			if(it == it_end) return {(std::uint32_t)EOF, 0, 0};
			
			auto cp = utf8::next(it, it_end);
			
			if(cp == '\n'){
				++line;
				col = 0;
			}
			else
				++col;
			
			return {cp, line, col};
		};
		
		while(it != it_end){
			auto tok_start = it;
			
			auto[cp, line, col] = next_cp();
			
			if(u_isspace(cp))
				continue;
			else if(cp == (std::uint32_t)EOF)
				break;
			
			auto tok_line = line;
			auto tok_col = col;
			
			std::size_t tok_size = 0;
			
			token_type tok_type;
			
			if(u_isalpha(cp) || (cp == '_')){ // identifiers, keywords
				while(1){
					if(it == it_end) break;
					
					auto cp_peek = utf8::peek_next(it, it_end);
					if(!u_isalnum(cp_peek) && (cp_peek != '_'))
						break;
					
					auto cp_data = next_cp();
					cp = cp_data.cp;
					line = cp_data.cp;
					col = cp_data.col;
				}
				
				tok_size = std::distance(tok_start, it);
				
				if(is_keyword(std::string_view(tok_start, tok_size)))
					tok_type = token_type::keyword;
				else
					tok_type = token_type::id;
			}
			else if(u_isdigit(cp)){ // integers, reals
				bool zero_base = false;
				
				if(cp == '0') zero_base = true;
				
				if(it != it_end){
					while(1){
						if(it == it_end) break;
						
						auto cp_peek = utf8::peek_next(it, it_end);
						if(!u_isdigit(cp_peek)){
							if(zero_base){
								if(cp_peek == '.') tok_type = token_type::real;
								else if(cp_peek != 'x')
									break;
							}
							else if(tok_type == token_type::integer){
								if(cp_peek == '.') tok_type = token_type::real;
								else break;
							}
							else if(tok_type == token_type::real){
								if(cp_peek == '.') throw lexer_error{location{name, line, col, 1}, "multiple decimal points in real constant"};
								else break;
							}
							else
								break;
						}
						
						auto cp_data = next_cp();
						cp = cp_data.cp;
						line = cp_data.cp;
						col = cp_data.col;
					}
				}
				
				tok_type = token_type::integer;
				tok_size = std::distance(tok_start, it);
				
				if(tok_size == 2){
					switch(*(tok_start + 1)){
						case 'x': throw lexer_error{location{name, line, col, 2}, "no constant after hexidecimal base"};
						case '.': throw lexer_error{location{name, line, col, 2}, "real must have fractional part"};
						default: break;
					}
				}
					
			}
			else if(is_bracket(cp)){
				tok_type = token_type::bracket;
				tok_size = 1;
			}
			else if(op_type_from_str(std::string_view(tok_start, std::distance(tok_start, it)))){
				while(1){
					if(it == it_end) break;
					
					//auto cp_peek = utf8::peek_next(it, it_end);
					auto op_str_end = it;
					utf8::next(op_str_end, it_end);
					
					if(!op_type_from_str(std::string_view(tok_start, std::distance(tok_start, op_str_end))))
						break;
					
					auto cp_data = next_cp();
					cp = cp_data.cp;
					line = cp_data.cp;
					col = cp_data.col;
				}
				
				tok_type = token_type::op;
				tok_size = std::distance(tok_start, it);
			}
			else if(cp == ';'){
				tok_type = token_type::end;
				tok_size = 1;
			}
			else
				throw lexer_error{
					location{name, line, col, 1},
					fmt::format("what the fuck is this? ({})", cp)
				};
			
			ret.emplace_back(
				tok_type,
				std::string_view(tok_start, tok_size),
				location{name, tok_line, tok_col, tok_size}
			);
		}
		
		return ret;
	}
}
