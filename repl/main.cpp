#include <iostream>
#include <clocale>

#include <readline/readline.h>
#include <readline/history.h>

#include "fmt/printf.h"

#include "purson/types.hpp"
#include "purson/expressions.hpp"
#include "purson/lexer.hpp"
#include "purson/parser.hpp"

#ifdef _WIN32
// get your shit together windows
#define PURSON_DEFAULT_LOCALE "C"
#else
#define PURSON_DEFAULT_LOCALE "en_US.UTF-8"
#endif

int main(int argc, char *argv[]){
	std::setlocale(LC_ALL, PURSON_DEFAULT_LOCALE);
	
	std::string_view ver = "dev";
	auto types = purson::types(ver);
	
	fmt::print("Purson REPL.\n");
	
	char *input;
	
	rl_bind_key('\t', rl_complete);
	
	while(1){
		//fmt::print("> ");
		//std::fflush(stdout);
		
		input = readline("> ");
		if(!input){
			break;
		}
		
		add_history(input);
		
		std::string_view input_str_v(input);
		
		std::string input_str;
		
		if(input_str_v == "exit") break;
		else if(input_str_v.back() != ';')
			input_str = fmt::format("{};", input);
		else
			input_str = input_str_v;
		
		try{
			auto tokens = purson::lex(ver, "REPL", input_str);
			auto exprs = purson::parse(ver, tokens);
			
			fmt::print("{} expressions\n", exprs.size());
			for(auto &&expr : exprs){
				if(auto val_expr = std::dynamic_pointer_cast<purson::integer_literal_expr>(expr)){
					fmt::print("\tInteger literal -> {}\n", val_expr->value_type()->str());
				}
			}
		}
		catch(const purson::lexer_error &err){
			fmt::print("[ERROR] {}\n", err.what());
			continue;
		}
		catch(const std::exception &err){
			fmt::print("[?????] {}\n", err.what());
		}
		catch(...){
			throw;
		}
	}
}
