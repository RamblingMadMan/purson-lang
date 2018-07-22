#include <string>

#include "purson/module.hpp"

#include "llvm.hpp"

#include <llvm/ExecutionEngine/MCJIT.h>
#include <llvm/ExecutionEngine/Interpreter.h>
#include <llvm/IR/Mangler.h>
#include <llvm/IR/PassManager.h>
#include <llvm/IR/LegacyPassManager.h>
#include <llvm/IR/LegacyPassManagers.h>
#include <llvm/Support/DynamicLibrary.h>
#include <llvm/Support/raw_ostream.h>
#include <llvm/Target/TargetMachine.h>
#include <llvm/ExecutionEngine/ExecutionEngine.h>
#include <llvm/ExecutionEngine/JITSymbol.h>
#include <llvm/ExecutionEngine/RuntimeDyld.h>
#include <llvm/ExecutionEngine/RTDyldMemoryManager.h>
#include <llvm/ExecutionEngine/SectionMemoryManager.h>
#include <llvm/ExecutionEngine/Orc/GlobalMappingLayer.h>
#include <llvm/ExecutionEngine/Orc/CompileOnDemandLayer.h>
#include <llvm/ExecutionEngine/Orc/CompileUtils.h>
#include <llvm/ExecutionEngine/Orc/IRCompileLayer.h>
#include <llvm/ExecutionEngine/Orc/IRTransformLayer.h>
#include <llvm/ExecutionEngine/Orc/LambdaResolver.h>
#include <llvm/ExecutionEngine/Orc/RTDyldObjectLinkingLayer.h>
#include <llvm/ExecutionEngine/Orc/IndirectionUtils.h>
#include <llvm/ExecutionEngine/Orc/ExecutionUtils.h>
#include <llvm/Transforms/InstCombine/InstCombine.h>
#include <llvm/Transforms/Scalar.h>
#include <llvm/Transforms/Scalar/GVN.h>

namespace purson{
	struct llvm_dummy_t{
		llvm_dummy_t(){
			llvm::sys::DynamicLibrary::LoadLibraryPermanently(nullptr);
			LLVMInitializeNativeAsmParser();
			LLVMInitializeNativeAsmPrinter();
			LLVMInitializeNativeDisassembler();
			LLVMInitializeNativeTarget();
		}
	} static dummy;
	
	std::string mangle_fn_name(std::string_view name, const type *ret, const std::vector<const type*> params){
		using namespace std::string_literals;
		auto str = "f"s + std::to_string(params.size());
		
		if(!ret)
			throw type_error{"return type required for mangling"};

		str += ret->str();
		
		for(std::size_t i = 0; i < params.size(); i++){
			auto ty = params[i];
			if(!ty)
				throw type_error{"parameter has not type"};

			str += ty->str();
		}

		str += name;
		
		return str;
	}
	
	class llvm_module: public jit_module{
		public:
			llvm_module(std::string_view name)
				: m_mod(std::make_shared<llvm::Module>(name.data(), llvm_ctx)), m_global_state{m_mod.get()}{}
			
			~llvm_module(){}
			
			void *get_fn_ptr(std::string_view mangled_name) override{
				return nullptr;
			}
			
			void compile(const std::vector<std::shared_ptr<const expr>> &ast) override{
				std::vector<llvm::Value*> values;
				for(auto &&ptr : ast)
					values.emplace_back(llvm_compile(ptr.get(), &m_global_state));
			}
			
			std::shared_ptr<llvm::Module> module() noexcept{ return m_mod; }
			
		private:
			std::shared_ptr<llvm::Module> m_mod;
			llvm_state m_global_state;
	};
	
	class llvm_moduleset: public jit_moduleset{
		public:
			llvm_moduleset(): tm(llvm::EngineBuilder().selectTarget()), dl(tm->createDataLayout()),
			  objectLayer([](){ return std::make_shared<llvm::SectionMemoryManager>(); }),
			  compileLayer(objectLayer, llvm::orc::SimpleCompiler(*tm)),
			  optimizeLayer(compileLayer, [this](std::shared_ptr<llvm::Module> m){
				  return optimizeModule(std::move(m));
			  }),
			  compileCallbackManager(llvm::orc::createLocalCompileCallbackManager(tm->getTargetTriple(), 0)),
			  codLayer(
				  optimizeLayer,
				  [this](llvm::Function &f){ return std::set<llvm::Function*>{&f}; },
				  *compileCallbackManager,
				  llvm::orc::createLocalIndirectStubsManagerBuilder(tm->getTargetTriple())
			  ),
			  mapLayer(codLayer){
				//mapLayer.setGlobalMapping("pursonEcho", (std::uintptr_t)pursonEcho);
				//mapLayer.setGlobalMapping("pursonStr", (std::uintptr_t)pursonStr);
				//mapLayer.setGlobalMapping("pursonPrint", (std::uintptr_t)pursonPrint);
				//mapLayer.setGlobalMapping("pursonVecToStr", (std::uintptr_t)pursonVec4ToStr);

				//mod->setDataLayout(m_dl);
			}
			
			~llvm_moduleset(){
				
			}
			
			void *get_fn_ptr(std::string_view mangled_name) override{
				auto name = std::string(mangled_name);
				std::string llvmName;
				llvm::raw_string_ostream str(llvmName);
				llvm::Mangler::getNameWithPrefix(str, name, dl);
				auto fn = mapLayer.findSymbol(str.str(), false);
				auto bits = fn.getAddress().get();
				return reinterpret_cast<void*>(bits);
			}
			
			jit_module *create_module(std::string_view name, const std::vector<std::shared_ptr<const expr>> &ast) override{
				auto resolver = llvm::orc::createLambdaResolver(
					[&](const std::string &name){
						if(auto sym = mapLayer.findSymbol(name, false))
							return sym;
						return llvm::JITSymbol(nullptr);
					},
					[](const std::string &name){
						if(auto symAddr =
							llvm::RTDyldMemoryManager::getSymbolAddressInProcess(name)
							)
							return llvm::JITSymbol(symAddr, llvm::JITSymbolFlags::Exported);
						return llvm::JITSymbol(nullptr);
					}
				);
				
				auto &&mod = m_mods.emplace_back(std::unique_ptr<llvm_module>(static_cast<llvm_module*>(make_jit_module(name, ast).release())));
				auto &&module_handle = m_mod_handles.emplace_back(cantFail(mapLayer.addModule(static_cast<llvm_module*>(mod.get())->module(), std::move(resolver))));
				return m_mod_ptrs.emplace_back(mod.get());
			}
			
			bool destroy_module(const module *mod) noexcept override{
				auto res = std::find_if(begin(m_mods), end(m_mods), [mod](auto &&ptr){ return ptr.get() == mod; });
				if(res == end(m_mods))
					return false;
				
				auto dist = std::distance(begin(m_mods), res);
				
				cantFail(mapLayer.removeModule(m_mod_handles[dist]));
				
				m_mods.erase(res);
				m_mod_handles.erase(begin(m_mod_handles) + dist);
				m_mod_ptrs.erase(begin(m_mod_ptrs) + dist);
				return true;
			}
			
			std::size_t num_modules() const noexcept override{ return m_mod_ptrs.size(); }
			const jit_module *const *modules() const noexcept override{ return m_mod_ptrs.data(); }
			
		private:
			std::unique_ptr<llvm::TargetMachine> tm;
			const llvm::DataLayout dl;
			mutable llvm::orc::RTDyldObjectLinkingLayer objectLayer;
			mutable llvm::orc::IRCompileLayer<decltype(objectLayer), llvm::orc::SimpleCompiler> compileLayer;

			using optimize_function = std::function<std::shared_ptr<llvm::Module>(std::shared_ptr<llvm::Module>)>;

			std::unique_ptr<llvm::orc::JITCompileCallbackManager> compileCallbackManager;

			mutable llvm::orc::IRTransformLayer<decltype(compileLayer), optimize_function> optimizeLayer;
			mutable llvm::orc::CompileOnDemandLayer<decltype(optimizeLayer)> codLayer;
			mutable llvm::orc::GlobalMappingLayer<decltype(codLayer)> mapLayer;

			using module_handle_t = decltype(mapLayer)::ModuleHandleT;
			
			std::vector<std::unique_ptr<llvm_module>> m_mods;
			std::vector<module_handle_t> m_mod_handles;
			std::vector<jit_module*> m_mod_ptrs;
			
			std::shared_ptr<llvm::Module> optimizeModule(std::shared_ptr<llvm::Module> m){
				auto fpm = llvm::make_unique<llvm::legacy::FunctionPassManager>(m.get());
				fpm->add(llvm::createPromoteMemoryToRegisterPass());
				fpm->add(llvm::createInstructionCombiningPass());
				fpm->add(llvm::createReassociatePass());
				fpm->add(llvm::createGVNPass());
				fpm->add(llvm::createCFGSimplificationPass());
				fpm->doInitialization();

				for(auto &f : *m)
					fpm->run(f);

				fpm->doFinalization();
				return m;
			};
	};
	
	std::unique_ptr<jit_moduleset> make_jit_moduleset(){
		auto ret = new llvm_moduleset();
		return std::unique_ptr<jit_moduleset>(ret);
	}
	
	std::unique_ptr<jit_module> make_jit_module(std::string_view name, const std::vector<std::shared_ptr<const expr>> &ast){
		auto ret = new llvm_module(name);
		if(ast.size()) ret->compile(ast);
		return std::unique_ptr<jit_module>(ret);
	}
}
