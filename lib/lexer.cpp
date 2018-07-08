#include "purson/lexer.hpp"

#include "utf8.h"
#include "fmt/format.h"

namespace purson{
	struct cp_data_t{
		std::uint32_t cp;
		std::size_t line, col;
	};
	
	std::vector<token> lex(std::string_view ver, std::string_view name, std::string_view src, const std::locale &locale){
		std::vector<token> ret;
		
		auto it = begin(src);
		auto it_end = end(src);
		
		auto next_cp = [&it, it_end, line = (std::size_t)1, col = (std::size_t)0]() mutable -> cp_data_t{
			if(it == it_end) return {(std::uint32_t)EOF, 0, 0};
			
			auto cp = utf8::next(it, it_end);
			if(cp == '\n'){
				++line;
				col = 1;
			}
			else
				++col;
			
			return {cp, line, col};
		};
		
		while(it != it_end){
			auto tok_start = it;
			auto tok_end = tok_start;
			
			auto[cp, line, col] = next_cp();
			
			if(std::isalpha(cp, locale)){
				while(1){
					++tok_end;
					auto cp_peek = utf8::peek_next(it, it_end);
					if(!std::isalnum(cp_peek, locale))
						break;
					
					auto cp_data = next_cp();
					cp = cp_data.cp;
					line = cp_data.cp;
					col = cp_data.col;
				}
				
				std::size_t tok_len = std::distance(tok_start, tok_end);
				
				ret.emplace_back(
					token_type::id,
					std::string_view(tok_start, tok_len),
					location{name, line, col, tok_len}
				);
			}
			else if(std::isdigit(cp, locale)){
				while(1){
					++tok_end;
					auto cp_peek = utf8::peek_next(it, it_end);
					if(!std::isalnum(cp_peek, locale))
						break;
					
					auto cp_data = next_cp();
					cp = cp_data.cp;
					line = cp_data.cp;
					col = cp_data.col;
				}
				
				std::size_t tok_len = std::distance(tok_start, tok_end);
				
				ret.emplace_back(
					token_type::integer,
					std::string_view(tok_start, tok_len),
					location{name, line, col, tok_len}
				);
			}
			/*
			else if(std::ispunct(cp, locale)){
				
			}
			else if(std::iscntrl(cp, locale)){
				
			}
			*/
			else
				throw lexer_error{
					location{name, line, col, 1},
					fmt::format("invalid character({}) encountered", cp)
				};
		}
		
		return ret;
	}
}
