#include <llvm/IR/Verifier.h>

#include "../llvm.hpp"

namespace purson{
	llvm_fn_gen_t &llvm_compile_fn_decl(const fn_decl_expr *decl, llvm_state *state){
		bool is_full_decl = true;

		auto llvm_ret_ty = llvm_type(decl->return_type());
		std::vector<llvm::Type*> llvm_param_tys;
		std::vector<const type*> param_tys;
		llvm_param_tys.reserve(decl->params().size());
		param_tys.reserve(decl->params().size());
		for(auto &&param : decl->params()){
			if(!param.second){
				is_full_decl = false;
				throw module_error{"only explicit typing is currently supported"};
			}
			llvm_param_tys.push_back(llvm_type(param_tys.emplace_back(param.second)));
		}

		auto llvm_fn_ty = llvm::FunctionType::get(llvm_ret_ty, llvm_param_tys, false);

		auto mangled = mangle_fn_name(decl->name(), decl->return_type(), param_tys);

		if(is_full_decl){
			if(decl->visibility() == fn_visibility::imported){
				if(auto fn = state->module()->getFunction(mangled)){
					fn->setLinkage(llvm::GlobalValue::LinkageTypes::AvailableExternallyLinkage);

					for(std::size_t i = 0; i < decl->params().size(); i++){
						auto arg = fn->args().begin() + i;
						arg->setName(std::string(decl->params()[i].first));
					}

					state->set_mangled_fn(mangled, fn);
					auto ret = [decl, fn, mangled](const type *ret_type_, const std::vector<const type*> &param_tys_){
						std::vector<const type*> param_tys;
						param_tys.reserve(decl->params().size());
						if(!ret_type_ && param_tys_.empty())
							return fn;
						else{
							if(decl->return_type()){
								if(ret_type_){
									if(ret_type_ != decl->return_type())
										throw module_error{"bad return type substitution"};
								} else
									ret_type_ = decl->return_type();
							} else if(!ret_type_)
								throw module_error{fmt::format("no return type given in reference to '{}'", decl->name())};

							for(std::size_t i = 0; i < decl->params().size(); i++){
								if(param_tys_.size() > i){
									if(param_tys_[i]){
										if(decl->params()[i].second && (param_tys_[i] != decl->params()[i].second))
											throw module_error{fmt::format("invalid parameter type given for parameter {}", i + 1)};
										else
											param_tys.emplace_back(param_tys_[i]);
									}
									else{
										if(!decl->params()[i].second)
											throw module_error{fmt::format("substitution required for parameter {}", i + 1)};

										param_tys.emplace_back(decl->params()[i].second);
									}
								}
								else if(!decl->params()[i].second)
									throw module_error{fmt::format("substitution required for parameter {}", i + 1)};
								else
									param_tys.emplace_back(decl->params()[i].second);
							}

							auto new_mangled = mangle_fn_name(decl->name(), ret_type_, param_tys);
							if(mangled == new_mangled)
								return fn;
							else
								throw module_error{fmt::format("bad function declaration substitution for '{}'", decl->name())};
						}
					};

					return state->add_fn(decl->name(), std::move(ret));
				}
			}
		}

		auto ret = [decl, param_tys, llvm_ret_ty, llvm_fn_ty, state](const type *ret_type_, const std::vector<const type*> &param_tys_){
			std::vector<const type*> criteria{ret_type_};
			criteria.reserve(param_tys_.size() + 1);
			criteria.insert(end(criteria), begin(param_tys_), end(param_tys_));

			if(decl->return_type()){
				if(ret_type_ != decl->return_type())
					throw module_error{"different return type used in declaration call"};
			}
			else if(!ret_type_)
				throw module_error{"different return type used in declaration call"};
			else
				ret_type_ = decl->return_type();

			auto mangled = mangle_fn_name(decl->name(), ret_type_, param_tys_);
			auto fn = state->get_mangled_fn(mangled);
			if(!fn)
				throw module_error{"function not defined"};

			return fn;
		};

		return state->add_fn(mangled, std::move(ret));
	}

	llvm_fn_gen_t &llvm_compile_fn_def(const fn_def_expr *def, llvm_state *state){
		bool is_full_def = true;

		if(!def->return_type())
			is_full_def = false;
		else{
			for(auto &&param : def->params()){
				if(!param.second){
					is_full_def = false;
					break;
				}
			}
		}

		auto ret = [&is_full_def, def, state, matches = std::map<std::string, llvm::Function*>{}](const type *ret_ty_, const std::vector<const type*> &param_tys_) mutable{
			// TODO: move type calculation out of functor. just checking where needed
			auto ret_ty = def->return_type();
			auto llvm_ret_ty = llvm_type(ret_ty);
			if(ret_ty_){
				auto llvm_ret_ty_ = llvm_type(ret_ty_);
				if(llvm_ret_ty){
					if(llvm_ret_ty != llvm_ret_ty_)
						throw module_error{"tried to substitute function with wrong return type"};
				}
				else{
					ret_ty = ret_ty_;
					llvm_ret_ty = llvm_ret_ty_;
				}
			}

			std::vector<const type*> param_tys;
			std::vector<llvm::Type*> llvm_param_tys;
			param_tys.reserve(def->params().size());
			llvm_param_tys.reserve(def->params().size());
			for(std::size_t i = 0; i < def->params().size(); i++){
				const type *param_ty = def->params()[i].second;
				llvm::Type *llvm_param_ty = nullptr;
				if(param_ty){
					llvm_param_ty = llvm_type(param_ty);

					if(param_tys_.size() > i){
						param_ty = param_tys_[i];
						if(llvm_param_ty != llvm_type(param_ty))
							throw module_error{"tried to substitute function with invalid parameter type"};
					}
				}
				else if(param_tys_.size() > i){
					param_ty = param_tys_[i];
					llvm_param_ty = llvm_type(param_ty);
					if(!param_ty || !llvm_param_ty)
						throw module_error{"parameter must be substituted"};
				}
				else
					throw module_error{"no type could be inferred for parameter"};

				param_tys.push_back(param_ty);
				llvm_param_tys.push_back(llvm_param_ty);
			}

			auto mangled_name = mangle_fn_name(def->name(), ret_ty, param_tys, def->linkage());
			auto res = matches.find(mangled_name);
			if(res != end(matches))
				return res->second;

			auto fn = state->get_fn(def->name());
			if(fn){
				throw module_error{"llvm function declaration-definition matching not implemented"};
				/*
				for(auto fn : fns){
				}
				*/
			}
			else{
				auto fn_ty = llvm::FunctionType::get(llvm_ret_ty, llvm_param_tys, false);

				auto llvm_linkage = llvm::GlobalValue::LinkageTypes::ExternalLinkage;

				if(def->visibility() == fn_visibility::local) llvm_linkage = llvm::GlobalValue::LinkageTypes::InternalLinkage;
				else if(def->visibility() == fn_visibility::imported) llvm_linkage = llvm::GlobalValue::LinkageTypes::AvailableExternallyLinkage; // ExternalWeakLinkage?

				fn = llvm::Function::Create(fn_ty, llvm_linkage, mangled_name, state->module());
				//fn->setCallingConv(llvm::CallingConv::C);
				//fn->setOnlyAccessesArgMemory();

				auto emplace_res = matches.emplace(mangled_name, fn);
				if(!emplace_res.second)
					throw module_error{"could not emplace function in state"};

				auto bb = llvm::BasicBlock::Create(llvm_ctx, "body", fn);
				llvm::IRBuilder<> builder(llvm_ctx);
				builder.SetInsertPoint(bb);

				llvm_state fn_state(state->module(), state, &builder);
				for(std::size_t i = 0; i < def->params().size(); i++){
					auto arg = fn->arg_begin() + i;
					arg->setName(std::string(def->params()[i].first));
					fn_state.set_var(def->params()[i].first, param_tys[i], arg);
				}

				if(!llvm::verifyFunction(*fn))
					throw module_error{fmt::format("error validating function {}", def->name())};

				try{
					if(auto block = dynamic_cast<const block_expr*>(def->body())){
						for(auto &&expr : block->exprs()){
							auto val = llvm_compile_rvalue(expr.get(), &fn_state);
						}
					}
					else if(auto rvalue = dynamic_cast<const rvalue_expr*>(def->body()))
						auto body_val = llvm_compile_rvalue(rvalue, &fn_state);
					else
						throw module_error{fmt::format("unexpected return expression '{}'", def->body()->str())};

					if(dynamic_cast<const unit_type*>(def->return_type()))
						builder.CreateRetVoid();

					state->set_mangled_fn(mangled_name, fn);

					return fn;
				}
				catch(const module_error &err){
					throw module_error{fmt::format("compile fn def ->\n\t{}", err.what())};
				}
				catch(...){
					throw;
				}
			}
		};

		if(is_full_def) ret(nullptr, {});

		return state->add_fn(def->name(), std::move(ret));
	}
}