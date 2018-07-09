#include <iostream>
#include <clocale>

#include <readline/readline.h>
#include <readline/history.h>

#include "fmt/printf.h"

#include "purson/lexer.hpp"

#ifdef _WIN32
// get your shit together windows
#define PURSON_DEFAULT_LOCALE "C"
#else
#define PURSON_DEFAULT_LOCALE "en_US.UTF-8"
#endif

int main(int argc, char *argv[]){
	std::setlocale(LC_ALL, PURSON_DEFAULT_LOCALE);
	
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
		
		std::string_view input_str(input);
		
		if(input_str == "exit") break;
		
		std::vector<purson::token> tokens;
		
		try{
			tokens = purson::lex("", "REPL", input_str);
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
		
		fmt::print("{} tokens\n", tokens.size());
		for(auto &&tok : tokens)
			fmt::print("\t{}\n", tok.str());
	}
}
