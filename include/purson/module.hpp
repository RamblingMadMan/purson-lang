#ifndef PURSON_MODULE_HPP
#define PURSON_MODULE_HPP 1

#include <vector>
#include <memory>

#include "expressions/base.hpp"

namespace purson{
	class module{
		public:
			virtual ~module() = default;
			virtual void *get_fn_ptr(std::string_view mangled_name) = 0;
	};
	
	class object_module: public virtual module{};
	
	class repl_module: public virtual module{};
	
	std::unique_ptr<repl_module> make_repl_module(const std::vector<std::shared_ptr<expr>> &ast = {});
	std::unique_ptr<object_module> make_object_module(const std::vector<std::shared_ptr<expr>> &ast = {});
}

#endif // !PURSON_MODULE_HPP
