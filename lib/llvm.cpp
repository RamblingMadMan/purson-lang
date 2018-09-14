#include <purson/expressions/op.hpp>
#include "llvm.hpp"

namespace purson{
	llvm::Value *llvm_compile(const expr *expr_, llvm_state *state){
		if(auto rvalue = dynamic_cast<const rvalue_expr*>(expr_))
			return llvm_compile_rvalue(rvalue, state);
		else
			throw module_error{fmt::format("unexpected valueless expression '{}'", expr_->str())};
	}

	llvm::Value *llvm_compile_rvalue(const rvalue_expr *rvalue, llvm_state *state){
		if(auto lit = dynamic_cast<const literal_expr*>(rvalue))
			return llvm_compile_literal(lit, state);
		else if(auto binop = dynamic_cast<const binary_op_expr*>(rvalue)){
			return llvm_compile_binop(binop, state);
		}
		else if(auto var_ref = dynamic_cast<const var_ref_expr*>(rvalue)){
			auto var = state->get_var(var_ref->name());
			if(!var) throw module_error{fmt::format("identifier '{}' does not refer to anything", var_ref->name())};
			return var->second;
		}
		else if(auto var_def = dynamic_cast<const var_def_expr*>(rvalue))
			return llvm_compile_var_def(var_def, state);
		else if(auto var_decl = dynamic_cast<const var_decl_expr*>(rvalue))
			return llvm_compile_var_decl(var_decl, state);
		else if(auto decl = dynamic_cast<const fn_decl_expr*>(rvalue)){
			llvm_compile_fn_decl(decl, state);
			return nullptr;
		}
		else if(auto def = dynamic_cast<const fn_def_expr*>(rvalue)){
			llvm_compile_fn_def(def, state);
			return nullptr;
		}
		else if(auto call = dynamic_cast<const fn_call_expr*>(rvalue)){
			if(!state->builder())
				throw module_error{"function call expression outside of a function body"};

			return llvm_compile_fn_call(call, state);
		}
		else if(auto ret = dynamic_cast<const return_expr*>(rvalue)){
			if(!state->builder())
				throw module_error{"return expression outside of a function body"};

			if(dynamic_cast<const unit_type*>(ret->value()->value_type()))
				return state->builder()->CreateRetVoid();
			else{
				try{
					auto llvm_ret_val = llvm_compile_rvalue(ret->value(), state);
					return state->builder()->CreateRet(llvm_ret_val);
				}
				catch(const module_error &err){
					throw module_error{fmt::format("compile return value -> \n\t{}", err.what())};
				}
				catch(...){
					throw;
				}
			}
		}
		else{
			throw module_error{fmt::format("unexpected {} value expression '{}'", rvalue->value_type()->str(), rvalue->str())};
		}
	}

	llvm::Value *llvm_compile_binop(const binary_op_expr *binop, llvm_state *state){
		auto lhs_val = llvm_compile_rvalue(binop->lhs().get(), state);
		auto rhs_val = llvm_compile_rvalue(binop->rhs().get(), state);

		auto lhs_ty = binop->lhs()->value_type();
		auto rhs_ty = binop->rhs()->value_type();

		auto higher_ty = promote_type(lhs_ty, rhs_ty);
		auto llvm_ty = llvm_type(higher_ty);

		if(lhs_ty == higher_ty){
			if(auto nat = dynamic_cast<const natural_type*>(lhs_ty)){
				rhs_val = state->builder()->CreateIntCast(rhs_val, llvm_ty, false);
			}
			else if(auto int_ = dynamic_cast<const integer_type*>(lhs_ty)){
				if(dynamic_cast<const natural_type*>(rhs_ty)){
					rhs_val = state->builder()->CreateCast(llvm::CastInst::CastOps::SExt, rhs_val, llvm_ty);
				}
				else{
					rhs_val = state->builder()->CreateIntCast(rhs_val, llvm_ty, true);
				}
			}
			else if(auto rational = dynamic_cast<const rational_type*>(lhs_ty)){
				throw module_error{"rational compilation currently unsupported"};
			}
			else if(auto real = dynamic_cast<const real_type*>(lhs_ty)){
				if(dynamic_cast<const rational_type*>(rhs_ty)){
					throw module_error{"rational compilation currently unsupported"};
				}
				else if(dynamic_cast<const integer_type*>(rhs_ty)){
					rhs_val = state->builder()->CreateCast(llvm::CastInst::CastOps::SIToFP, rhs_val, llvm_ty);
				}
				else if(dynamic_cast<const natural_type*>(rhs_ty)){
					rhs_val = state->builder()->CreateCast(llvm::CastInst::CastOps::UIToFP, rhs_val, llvm_ty);
				}
				else{
					rhs_val = state->builder()->CreateCast(llvm::CastInst::CastOps::SIToFP, rhs_val, llvm_ty);
				}
			}
			else
				throw module_error{"unexpected type in compilation"};
		}
		else{
			if(auto nat = dynamic_cast<const natural_type*>(rhs_ty)){
				lhs_val = state->builder()->CreateIntCast(lhs_val, llvm_ty, false);
			}
			else if(auto int_ = dynamic_cast<const integer_type*>(rhs_ty)){
				if(dynamic_cast<const natural_type*>(lhs_ty)){
					lhs_val = state->builder()->CreateCast(llvm::CastInst::CastOps::SExt, lhs_val, llvm_ty);
				}
				else{
					lhs_val = state->builder()->CreateIntCast(lhs_val, llvm_ty, true);
				}
			}
			else if(auto rational = dynamic_cast<const rational_type*>(rhs_ty)){
				throw module_error{"rational compilation currently unsupported"};
			}
			else if(auto real = dynamic_cast<const real_type*>(rhs_ty)){
				if(dynamic_cast<const rational_type*>(lhs_ty)){
					throw module_error{"rational compilation currently unsupported"};
				}
				else if(dynamic_cast<const integer_type*>(lhs_ty)){
					lhs_val = state->builder()->CreateCast(llvm::CastInst::CastOps::SIToFP, lhs_val, llvm_ty);
				}
				else if(dynamic_cast<const natural_type*>(lhs_ty)){
					lhs_val = state->builder()->CreateCast(llvm::CastInst::CastOps::UIToFP, lhs_val, llvm_ty);
				}
				else{
					lhs_val = state->builder()->CreateCast(llvm::CastInst::CastOps::SIToFP, lhs_val, llvm_ty);
				}
			}
			else
				throw module_error{"unexpected type in compilation"};
		}

		switch(binop->operator_().op_type()){
			case operator_type::add:{
				if(dynamic_cast<const real_type*>(higher_ty))
					return state->builder()->CreateFAdd(lhs_val, rhs_val);
				else
					return state->builder()->CreateAdd(lhs_val, rhs_val);
			}
			case operator_type::sub:{
				if(dynamic_cast<const real_type*>(higher_ty))
					return state->builder()->CreateFSub(lhs_val, rhs_val);
				else
					return state->builder()->CreateSub(lhs_val, rhs_val);
			}
			case operator_type::mul:{
				if(dynamic_cast<const real_type*>(higher_ty))
					return state->builder()->CreateFMul(lhs_val, rhs_val);
				else
					return state->builder()->CreateMul(lhs_val, rhs_val);
			}
			case operator_type::div:{
				if(dynamic_cast<const natural_type*>(higher_ty))
					return state->builder()->CreateUDiv(lhs_val, rhs_val);
				else if(dynamic_cast<const integer_type*>(higher_ty))
					return state->builder()->CreateSDiv(lhs_val, rhs_val);
				else if(dynamic_cast<const real_type*>(higher_ty)){
					return state->builder()->CreateFDiv(lhs_val, rhs_val);
				}
			}

			default:
				throw module_error{"unsupported binary operator"};
		}
	}
	
	llvm::Value *llvm_compile_fn_call(const fn_call_expr *call, llvm_state *state){
		if(call->args().size()){
			std::vector<const type *> subs{call->fn()->return_type()};
			std::vector<llvm::Value *> llvm_arg_values;
			llvm_arg_values.reserve(subs.size());
			subs.reserve(call->args().size() + 1);
			for(auto &&arg : call->args()){
				subs.push_back(arg->value_type());
				llvm_arg_values.push_back(llvm_compile(arg.get(), state));
			}

			subs.erase(begin(subs));
			auto mangled = mangle_fn_name(call->fn()->name(), call->fn()->return_type(), subs);
			auto fn = state->get_mangled_fn(mangled);

			auto fn_ty = fn->getFunctionType();
			if(!fn_ty->getReturnType())
				throw module_error{fmt::format("un-fulfilled function identifier {} with null return type", call->fn()->name())};

			for(std::size_t i = 0; i < call->fn()->params().size(); i++){
				auto arg = fn->arg_begin() + i;
				if(!arg->getType())
					throw module_error{
						fmt::format(
							"un-fulfilled function identifier {} with null type for parameter {}",
							call->fn()->name(), call->fn()->params()[i].first
						)
					};
			}

			return state->builder()->CreateCall(fn, llvm_arg_values);
		}
		else{
			auto mangled = mangle_fn_name(call->fn()->name(), call->fn()->return_type(), {});
			auto fn = state->get_mangled_fn(mangled);
			return state->builder()->CreateCall(fn);
		}
	}
	
	llvm::Constant *llvm_compile_literal(const literal_expr *lit, llvm_state *state){
		if(auto num = dynamic_cast<const numeric_literal_expr*>(lit)){
			if(auto nat_lit = dynamic_cast<const natural_literal_expr*>(num)){
				std::string val_str;
				auto val = nat_lit->value();
				val_str.resize(mpz_sizeinbase(val, 10) + 1);
				mpz_get_str(&val_str[0], 10, val);
				
				return llvm::ConstantInt::get(llvm_ctx, llvm::APInt(nat_lit->value_type()->bits(), val_str, 10));
			}
			else if(auto int_lit = dynamic_cast<const integer_literal_expr*>(num)){
				std::string val_str;
				auto val = int_lit->value();
				val_str.resize(mpz_sizeinbase(val, 10) + 1);
				mpz_get_str(&val_str[0], 10, val);
				
				return llvm::ConstantInt::get(llvm_ctx, llvm::APInt(int_lit->value_type()->bits(), val_str, 10));
			}
			else if(auto rat_lit = dynamic_cast<const rational_literal_expr*>(num)){
				auto val = rat_lit->value();
				
				mpz_t num, denom;
				mpz_inits(num, denom, NULL);
				
				mpq_get_num(num, val);
				mpq_get_den(denom, val);
				
				std::string num_str, denom_str;
				num_str.resize(mpz_sizeinbase(num, 10) + 1);
				denom_str.resize(mpz_sizeinbase(denom, 10) + 1);
				
				mpz_get_str(&num_str[0], 10, num);
				mpz_get_str(&denom_str[0], 10, denom);
				
				mpz_clears(num, denom, NULL);
				
				auto num_constant = llvm::ConstantInt::get(llvm_ctx, llvm::APInt(rat_lit->value_type()->bits() / 2, num_str, 10));
				auto denom_constant = llvm::ConstantInt::get(llvm_ctx, llvm::APInt(rat_lit->value_type()->bits() / 2, denom_str, 10));
				
				return llvm::ConstantVector::get({num_constant, denom_constant});
			}
			else if(auto real_lit = dynamic_cast<const real_literal_expr*>(num)){
				auto val = real_lit->value();
				fmt::print(stderr, "WARNING: real literals currently shortened to long double\n");
				auto constant_val = mpfr_get_d(val, MPFR_RNDN);
				return llvm::ConstantFP::get(llvm_ctx, llvm::APFloat(constant_val));
			}
			else
				throw module_error{"unexpected numeric literal expression type"};
		}
		else
			throw module_error{"unexpected literal expression type"};
	}
}
