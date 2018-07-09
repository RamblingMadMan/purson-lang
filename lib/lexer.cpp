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
			auto tok_end = tok_start;
			
			auto[cp, line, col] = next_cp();
			
			auto tok_line = line;
			auto tok_col = col;
			
			std::size_t tok_size = 0;
			
			token_type tok_type = token_type::end;
			
			if(u_isspace(cp))
				continue;
			else if(cp == (std::uint32_t)EOF)
				break;
			else if(u_isalpha(cp)){ // identifiers, keywords
				while(1){
					if(it == it_end) break;
					
					utf8::next(tok_end, it_end);
					
					auto cp_peek = utf8::peek_next(it, it_end);
					if(!u_isalnum(cp_peek))
						break;
					
					auto cp_data = next_cp();
					cp = cp_data.cp;
					line = cp_data.cp;
					col = cp_data.col;
				}
				
				tok_type = token_type::id;
				tok_size = std::distance(tok_start, tok_end);
			}
			else if(u_isdigit(cp)){ // integers, reals
				bool zero_base = false;
				
				if(cp == '0') zero_base = true;
				
				if(it != it_end){
					tok_type = token_type::integer;
					
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
				
				tok_size = std::distance(tok_start, it);
				
				if(tok_size == 2){
					switch(*(tok_start + 1)){
						case 'x': throw lexer_error{location{name, line, col, 2}, "no constant after hexidecimal base"};
						case '.': throw lexer_error{location{name, line, col, 2}, "real must have fractional part"};
						default: break;
					}
				}
					
			}
			else if(op_type_from_str(std::string_view(tok_start, std::distance(tok_start, it)))){
				if(cp != ';'){
					while(1){
						if(it == it_end) break;
						
						utf8::next(tok_end, it_end);
						
						auto cp_peek = utf8::peek_next(it, it_end);
						auto op_str_end = tok_end;
						utf8::next(op_str_end, it_end);
						
						if(!u_ispunct(cp) || !op_type_from_str(std::string_view(tok_start, std::distance(tok_start, op_str_end))))
							break;
						
						auto cp_data = next_cp();
						cp = cp_data.cp;
						line = cp_data.cp;
						col = cp_data.col;
					}
					
					tok_type = token_type::op;
					tok_size = std::distance(tok_start, it);
				}
			}
			else
				throw lexer_error{
					location{name, line, col, 1},
					fmt::format("invalid character({}) encountered", cp)
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
