#ifndef PURSON_MODULE_HPP
#define PURSON_MODULE_HPP 1

#include <vector>
#include <memory>

#include "exception.hpp"
#include "expressions/base.hpp"

namespace purson{
	class module_error: public exception{ using exception::exception; };
	
	class module{
		public:
			virtual ~module() = default;
			virtual void *get_fn_ptr(std::string_view mangled_name) = 0;
	};
	
	class jit_module: public virtual module{
		public:
			virtual void compile(const std::vector<std::shared_ptr<expr>> &ast) = 0;
	};
	
	std::unique_ptr<jit_module> make_jit_module(std::string_view name, const std::vector<std::shared_ptr<expr>> &ast = {});
	std::unique_ptr<module> make_static_module(std::string_view name, const std::vector<std::shared_ptr<expr>> &ast);
}

#endif // !PURSON_MODULE_HPP
