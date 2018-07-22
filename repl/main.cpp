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
	auto modules = purson::make_jit_moduleset();

	using repl_fn_t = void(*)();

	auto repl_ret_ty = types->unit();

	auto repl_mangled_name = purson::mangle_fn_name("repl", repl_ret_ty, {});
	
	fmt::print("Purson REPL\n");
	
	char *input;
	
	rl_bind_key('\t', rl_complete);
	
	std::string src = "fn repl() -> Unit{\n";
	
	purson::jit_module *module = nullptr;

	std::vector<std::shared_ptr<const purson::fn_expr>> fn_exprs;
	std::vector<std::shared_ptr<const purson::expr>> repl_exprs;
	
	while(1){
		input = readline("> ");
		if(!input){
			break;
		}
		
		add_history(input);
		
		std::string_view input_str_v(input);
		
		std::string input_str;
		
		if(input_str_v == "exit") break;
		else if(input_str_v.back() != ';')
			input_str = fmt::format("{};", input_str_v);
		else
			input_str = input_str_v;
		
		src += input_str;
		auto final_src = fmt::format("{}\n{}", src, '}');
		
		try{
			modules->destroy_module(module);
			
			auto tokens = purson::lex(ver, "repl", final_src);
			//fmt::print(stderr, "Tokens done\n");
			
			auto exprs = purson::parse_repl(ver, tokens, types);
			//fmt::print(stderr, "AST done\n");

			for(auto &&expr : exprs){
				if(auto fn_ = std::dynamic_pointer_cast<const purson::fn_expr>(expr))
					fn_exprs.push_back(std::move(fn_));
				else
					repl_exprs.push_back(expr);
			}
			
			module = modules->create_module("repl", exprs);
			//fmt::print(stderr, "Module done\n");
			
			auto repl_fn_vptr = modules->get_fn_ptr(repl_mangled_name);
			if(!repl_fn_vptr)
				throw std::runtime_error{fmt::format("couldn't find {} in the moduleset", repl_mangled_name)};

			reinterpret_cast<repl_fn_t>(repl_fn_vptr)();

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
		catch(const purson::module_error &err){
			fmt::print("[MODULE ERROR] {}\n", err.what());
		}
		catch(const std::exception &err){
			fmt::print("[?????] {}\n", err.what());
		}
		catch(...){
			throw;
		}
	}
}
