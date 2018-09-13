#ifndef PURSON_LIB_LLVM_HPP
#define PURSON_LIB_LLVM_HPP 1

#include <optional>

#include "fmt/format.h"

#include <llvm/ADT/STLExtras.h>
#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/DataLayout.h>
#include <llvm/Linker/Linker.h>

#include "purson/module.hpp"
#include "purson/expressions/literal.hpp"
#include "purson/expressions/function.hpp"
#include "purson/expressions/var.hpp"
#include "purson/expressions/op.hpp"
#include "purson/types/numeric.hpp"

namespace purson{
	inline thread_local llvm::LLVMContext llvm_ctx;

	inline llvm::Type *llvm_type(const unit_type*){ return llvm::Type::getVoidTy(llvm_ctx); }

	inline llvm::Type *llvm_type(const type_type*){
		std::vector<llvm::Type*> elem_types{
			llvm::Type::getInt8PtrTy(llvm_ctx),
			llvm::Type::getInt64Ty(llvm_ctx)
		};
		return llvm::StructType::get(llvm_ctx, elem_types);
	}

	inline llvm::IntegerType *llvm_type(const integer_type *integer_ty){ return llvm::Type::getIntNTy(llvm_ctx, integer_ty->bits()); }
	inline llvm::IntegerType *llvm_type(const natural_type *natural_ty){ return llvm::Type::getIntNTy(llvm_ctx, natural_ty->bits()); }

	inline llvm::VectorType *llvm_type(const rational_type *rational_ty){
		auto int_ty = llvm::Type::getIntNTy(llvm_ctx, rational_ty->bits() / 2);
		return llvm::VectorType::get(int_ty, 2);
	}

	inline llvm::Type *llvm_type(const real_type *real_ty){
		switch(real_ty->bits()){
			case 16: return llvm::Type::getHalfTy(llvm_ctx);
			case 32: return llvm::Type::getFloatTy(llvm_ctx);
			case 64: return llvm::Type::getDoubleTy(llvm_ctx);
			default: throw module_error{fmt::format("could not get llvm type for real type '{}'", real_ty->str())};
		}
	}

	inline llvm::Type *llvm_type(const type *ty){
		if(!ty) return nullptr;
		else if(auto unit = dynamic_cast<const unit_type*>(ty)) return llvm_type(unit);
		else if(auto int_ = dynamic_cast<const integer_type*>(ty)) return llvm_type(int_);
		else if(auto natural = dynamic_cast<const natural_type*>(ty)) return llvm_type(natural);
		else if(auto rational = dynamic_cast<const rational_type*>(ty)) return llvm_type(rational);
		else if(auto real = dynamic_cast<const real_type*>(ty)) return llvm_type(real);
		else if(auto type = dynamic_cast<const type_type*>(ty)) return llvm_type(type);
		throw module_error{fmt::format("could not get llvm type for type '{}'", ty->str())};
	}
	
	using llvm_fn_gen_t = std::function<llvm::Function*(const type*, const std::vector<const type*>&)>;
	
	class llvm_state{
		public:
			llvm_state(llvm::Module *module_, const llvm_state *parent_ = nullptr, llvm::IRBuilder<> *builder_ = nullptr)
				: m_module(module_), m_parent(parent_), m_builder{builder_}{}
			
			~llvm_state(){}
			
			llvm::Module *module() noexcept{ return m_module; }
			const llvm_state *parent() const noexcept{ return m_parent; }
			llvm::IRBuilder<> *builder() noexcept{ return m_builder; }
			
			llvm::Function *get_fn(std::string_view name, const type *ret_ty = nullptr, const std::vector<const type*> &param_tys = {}){
				auto res = m_fn_defs.find(name);
				if(res != end(m_fn_defs))
					return res->second(ret_ty, param_tys);
				
				return nullptr;
			}
			
			std::optional<std::pair<const type*, llvm::Value*>> get_var(std::string_view name){
				auto res = m_vars.find(name);
				if(res != end(m_vars))
					return res->second;
				
				return std::nullopt;
			}
			
			void set_var(std::string_view name, const type *ty, llvm::Value *val){
				m_vars[name] = std::make_pair(ty, val);
			}
			
			llvm_fn_gen_t &add_fn(std::string_view name, llvm_fn_gen_t &&gen){
				return m_fn_defs[name] = std::move(gen);
			}
			
			llvm::Function *get_mangled_fn(const std::string &mangled_name) const{
				auto res = m_mangled_fns.find(mangled_name);
				if(res != end(m_mangled_fns))
					return res->second;
				else if(m_parent)
					return m_parent->get_mangled_fn(mangled_name);
				else
					return m_module->getFunction(mangled_name);
			}
			
			void set_mangled_fn(std::string_view mangled_name, llvm::Function *llvm_fn){
				m_mangled_fns[mangled_name] = llvm_fn;
			}
			
		private:
			llvm::Module *m_module;
			const llvm_state *m_parent;
			llvm::IRBuilder<> *m_builder;
			
			std::map<std::string_view, std::pair<const type*, llvm::Value*>> m_vars;
			std::map<std::string_view, llvm_fn_gen_t> m_fn_defs;
			std::map<std::string_view, llvm::Function*> m_mangled_fns;
	};
	
	llvm::Value *llvm_compile(const expr *expr_, llvm_state *state);

	llvm::Value *llvm_compile_rvalue(const rvalue_expr *rvalue, llvm_state *state);
	llvm::Value *llvm_compile_binop(const binary_op_expr *binop, llvm_state *state);

	llvm::Value *llvm_compile_ret(const return_expr *ret, llvm_state *state);

	llvm::Value *llvm_compile_var_decl(const var_decl_expr *decl, llvm_state *state);
	llvm::Value *llvm_compile_var_def(const var_def_expr *def, llvm_state *state);
	
	llvm_fn_gen_t &llvm_compile_fn_decl(const fn_decl_expr *decl, llvm_state *state);
	llvm_fn_gen_t &llvm_compile_fn_def(const fn_def_expr *def, llvm_state *state);
	
	llvm::Value *llvm_compile_fn_call(const fn_call_expr *call, llvm_state *state);
	llvm::Constant *llvm_compile_literal(const literal_expr *lit, llvm_state *state);
}

#endif // !PURSON_LIB_LLVM_HPP
