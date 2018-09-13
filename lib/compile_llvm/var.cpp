#include "../llvm.hpp"

namespace purson{
	llvm::Value *llvm_compile_var_decl(const var_decl_expr *decl, llvm_state *state){
		if(!state->builder())
			throw module_error{"variable declaration outside of function body"};
		else if(state->get_var(decl->name()))
			throw module_error{"variable with same name already exists"};

		auto val_llvm = state->builder()->CreateAlloca(llvm_type(decl->value_type()));
		state->set_var(decl->name(), decl->value_type(), val_llvm);
		return val_llvm;
	}

	llvm::Value *llvm_compile_var_def(const var_def_expr *def, llvm_state *state){
		if(!state->builder())
			throw module_error{"variable declaration outside of function body"};
		else if(state->get_var(def->name()))
			throw module_error{"variable with same name already exists"};

		auto val_llvm = state->builder()->CreateAlloca(llvm_type(def->value_type()));
		state->set_var(def->name(), def->value_type(), val_llvm);
		auto rvalue_llvm = llvm_compile_rvalue(def->value().get(), state);
		state->builder()->CreateStore(rvalue_llvm, val_llvm);
		return val_llvm;
	}
}
