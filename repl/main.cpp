#include <iostream>
#include <clocale>

#include <readline/readline.h>
#include <readline/history.h>

#include "fmt/printf.h"

#include "purson/types.hpp"
#include "purson/expressions.hpp"
#include "purson/lexer.hpp"
#include "purson/parser.hpp"
#include "purson/module.hpp"

#ifdef _WIN32
// get your shit together windows
#define PURSON_DEFAULT_LOCALE "C"
#else
#define PURSON_DEFAULT_LOCALE "en_US.UTF-8"
#endif

void print_error_squigglies(const purson::location &loc){
	// + 1 because of prompt character
	for(std::size_t i = 0; i < loc.col() + 1; i++)
		fmt::print(" ");
	
	for(std::size_t i = 0; i < loc.len(); i++)
		fmt::print("~");
}

int main(int argc, char *argv[]){
	std::setlocale(LC_ALL, PURSON_DEFAULT_LOCALE);
	
	std::string_view ver = "dev";
	auto types = purson::types(ver);
	
	fmt::print("Purson REPL\n");
	
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
		
		std::unique_ptr<purson::jit_module> module;
		
		try{
			module = purson::make_jit_module("repl");
		}
		catch(const purson::module_error &err){
			fmt::print("[INTERNAL ERROR] {}\n", err.what());
		}
		catch(...){
			throw;
		}
		
		try{
			auto tokens = purson::lex(ver, "REPL", input_str);
			auto exprs = purson::parse_repl(ver, tokens, types);
			
			module->compile(exprs);
			
			fmt::print("{} {}\n", exprs.size(), exprs.size() > 1 ? "expressions" : "expression");
		}
		catch(const purson::lexer_error &err){
			print_error_squigglies(err.location());
			fmt::print("\n[LEXER ERROR] {}\n", err.what());
			continue;
		}
		catch(const purson::parser_error &err){
			print_error_squigglies(err.location());
			fmt::print("\n[PARSER ERROR] {}\n", err.what());
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
