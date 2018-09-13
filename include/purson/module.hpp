#ifndef PURSON_MODULE_HPP
#define PURSON_MODULE_HPP 1

#include <vector>
#include <memory>

#include "exception.hpp"
#include "expressions/function.hpp"
#include "expressions/base.hpp"

namespace purson{
	std::string mangle_fn_name(std::string_view name, const type *ret, const std::vector<const type*> params, fn_linkage linkage = fn_linkage::purson);
	
	class module_error: public exception{ using exception::exception; };

	enum class target{
		auto_, arm, arm64, x86, x86_64
	};

	class module{
		public:
			virtual ~module() = default;
			virtual void *get_fn_ptr(std::string_view mangled_name) = 0;

			virtual void write(std::string_view path) = 0;
	};
	
	class jit_module: public virtual module{
		public:
			virtual void compile(const std::vector<std::shared_ptr<const expr>> &ast) = 0;

			virtual void register_func(const std::string &name, void *fn_ptr, const function_type *fn_ty) = 0;
	};
	
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
			
			virtual jit_module *create_module(std::string_view name,  const std::vector<std::shared_ptr<const expr>> &exprs = {}) override = 0;
			virtual bool destroy_module(const module*) noexcept override = 0;

			virtual void set_fn_ptr(std::string_view identifier, void *fn_ptr) = 0;

			virtual void write(std::string_view path) = 0;
	};
	
	std::unique_ptr<jit_moduleset> make_jit_moduleset(target t = target::auto_);
}

#endif // !PURSON_MODULE_HPP
