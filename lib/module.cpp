#include <llvm/ADT/STLExtras.h>
#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/DataLayout.h>

#include "purson/module.hpp"

namespace purson{
	thread_local llvm::LLVMContext llvm_ctx;
	
	class llvm_module: public module{
		public:
			llvm_module(){}
			~llvm_module(){}
	};
}
