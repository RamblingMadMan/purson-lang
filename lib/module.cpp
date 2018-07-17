#include "purson/module.hpp"

#include "llvm.hpp"

namespace purson{
	class llvm_module: public jit_module{
		public:
			llvm_module(std::string_view name)
				: m_mod(name.data(), llvm_ctx), m_global_state{&m_mod}{}
			~llvm_module(){}
			
			void *get_fn_ptr(std::string_view mangled_name) override{
				return nullptr;
			}
			
			void compile(const std::vector<std::shared_ptr<expr>> &ast) override{
				std::vector<llvm::Value*> values;
				for(auto &&ptr : ast)
					values.emplace_back(llvm_compile(ptr.get(), &m_global_state));
			}
			
		private:
			llvm::Module m_mod;
			llvm_state m_global_state;
	};
	
	std::unique_ptr<jit_module> make_jit_module(std::string_view name, const std::vector<std::shared_ptr<expr>> &ast){
		auto ret = new llvm_module(name);
		if(ast.size()) ret->compile(ast);
		return std::unique_ptr<jit_module>(ret);
	}
}
