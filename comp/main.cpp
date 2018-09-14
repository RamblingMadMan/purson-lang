#include <vector>
#include <fstream>
#include <filesystem>

#include "fmt/format.h"

#include "purson/types.hpp"
#include "purson/lexer.hpp"
#include "purson/parser.hpp"
#include "purson/module.hpp"

extern "C"
void f1i32u0println(std::int32_t i){ fmt::print(stdout, "{}\n", i); }

int main(int argc, char *argv[]){
	std::vector<std::string> input_files;
	std::string_view output_file;
	std::string_view revision = "dev";

	for(int i = 1; i < argc; i++){
		auto arg = std::string_view(argv[i]);
		if(arg == "-o"){
			++i;
			if(i >= argc){
				fmt::print(stderr, "no output file given after '-o'\n");
				return EXIT_FAILURE;
			}

			output_file = std::string_view(argv[i]);
		}
		else if(arg == "-r"){
			++i;
			if(i >= argc){
				fmt::print(stderr, "no revision given after '-r'\n");
				return EXIT_FAILURE;
			}

			revision = arg;
		}
		else if(arg[0] == '-'){
			fmt::print(stderr, "invalid option specified\n");
			return EXIT_FAILURE;
		}
		else
			input_files.emplace_back(arg);
	}

	if(!input_files.size()){
		fmt::print(stderr, "no input files given. exiting...\n");
		return EXIT_FAILURE;
	}
	else if(!output_file.size()){
		fmt::print(stderr, "no output file given. exiting...\n");
		return EXIT_FAILURE;
	}

	auto types = purson::types(revision);
	auto modules = purson::make_jit_moduleset(purson::target::auto_);

	auto unit_ty = types->unit();
	auto int32_ty = types->integer(32);
	auto uint32_ty = types->natural(32);

	auto fn_ty = types->function(unit_ty, {int32_ty});

	auto main_fn_name = purson::mangle_fn_name("main", int32_ty, {});

	modules->set_fn_ptr("f1i32u0println", reinterpret_cast<void*>(f1i32u0println));

	std::vector<purson::jit_module*> jit_modules;
	jit_modules.reserve(input_files.size());

	namespace fs = std::filesystem;

	for(std::size_t i = 0; i < input_files.size(); i++){
		fs::path p(input_files[i]);
		if(!fs::exists(p)){
			fmt::print(stderr, "file \"{}\" doesn't exist\n", input_files[i]);
			return EXIT_FAILURE;
		}

		std::ifstream ifile(p);

		std::string src, tmp;
		while(std::getline(ifile, tmp)) src = fmt::format("{}\n{}", src, tmp);

		auto toks = purson::lex(revision, input_files[i], src);
		auto exps = purson::parse(revision, toks);

		auto &&module = jit_modules.emplace_back(modules->create_module(input_files[i]));
		module->register_func("f1i32u0println", reinterpret_cast<void*>(f1i32u0println), fn_ty);
		module->compile(exps);
		//module->write(output_file);
	}

	auto mainFn = modules->get_fn_ptr(main_fn_name);
	if(!mainFn){
		fmt::print(stderr, "could not find main function '{}'\n", main_fn_name);
	}
}
