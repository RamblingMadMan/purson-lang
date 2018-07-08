#include <iostream>

#include "fmt/printf.h"

#include "purson/lexer.hpp"

int main(int argc, char *argv[]){
	fmt::print("Purson REPL.\n");
	
	while(1){
		fmt::print("> ");
		std::fflush(stdout);
		
		std::string input;
		std::getline(std::cin, input);
		
		if(input == "exit") break;
		
		try{
			purson::lex("", "REPL", input);
		}
		catch(const purson::lexer_error &err){
			fmt::print("[ERROR] {}\n", err.what());
			continue;
		}
		catch(...){
			throw;
		}
	}
}
