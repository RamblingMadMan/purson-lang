#ifndef PURSON_MODULE_HPP
#define PURSON_MODULE_HPP 1

#include <vector>
#include <memory>

#include "exception.hpp"
#include "expressions/function.hpp"
#include "expressions/base.hpp"

namespace purson{
	std::string mangle_fn_name(std::string_view name, const type *ret, const std::vector<const type*> params);
	
	class module_error: public exception{ using exception::exception; };
	
	class module{
		public:
			virtual ~module() = default;
			virtual void *get_fn_ptr(std::string_view mangled_name) = 0;
	};
	
	class jit_module: public virtual module{
		public:
			virtual void compile(const std::vector<std::shared_ptr<const expr>> &ast) = 0;
	};
	
	std::unique_ptr<jit_module> make_jit_module(std::string_view name, const std::vector<std::shared_ptr<const expr>> &ast = {});
	std::unique_ptr<module> make_static_module(std::string_view name, const std::vector<std::shared_ptr<const expr>> &ast);
	
	class moduleset{
		public:
			virtual ~moduleset() = default;
			virtual module *create_module(std::string_view name, const std::vector<std::shared_ptr<const expr>>&) = 0;
			virtual bool destroy_module(const module*) noexcept = 0;
			virtual void *get_fn_ptr(std::string_view mangled_name) = 0;
	};
	
	class jit_moduleset: public moduleset{
		public:
			virtual std::size_t num_modules() const noexcept = 0;
			virtual const jit_module *const *modules() const noexcept = 0;
			
			virtual jit_module *create_module(std::string_view name, const std::vector<std::shared_ptr<const expr>>&) override = 0;
			virtual bool destroy_module(const module*) noexcept override = 0;
	};
	
	std::unique_ptr<jit_moduleset> make_jit_moduleset();
}

#endif // !PURSON_MODULE_HPP
