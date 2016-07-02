#pragma once

#include "LLVMInclude.h"
#include "Types.h"
#include <unordered_map>
#include "CompilerException.h"
#include "SymbolTable.h"

namespace emll
{
	namespace compiler
	{
		using IRValueList = TypeList<llvm::Value*>;
		using IRVariableTable = SymbolTable<llvm::Value>;

		class LLVMException : CompilerException<std::error_code>
		{
		public:
			LLVMException(std::error_code error)
				: CompilerException(error)
			{
			}
		};

		class IREmitter
		{
		public:
			IREmitter();

			llvm::Type* Type(const ValueType type);
			llvm::ArrayType* ArrayType(const ValueType type, uint64_t size);

			llvm::Constant* Literal(const unsigned char value);
			llvm::Constant* Literal(const short value);
			llvm::Constant* Literal(const int value);
			llvm::Constant* Literal(const int64_t value);
			llvm::Constant* Literal(const double value);
			llvm::Value* Literal(const std::string& value);
			llvm::Constant* Literal(const std::vector<double>& value);
			llvm::Value* Literal(const std::string& name, const std::string& value);
			llvm::Constant* Zero(const ValueType type);

			llvm::Value* Cast(llvm::Value* pValue, const ValueType destType);
			llvm::Value* CastFloat(llvm::Value* pValue, const ValueType destType);

			llvm::ReturnInst* ReturnVoid();
			llvm::ReturnInst* Return(llvm::Value* pValue);

			llvm::Value* BinaryOp(const OperatorType type, llvm::Value* pLVal, llvm::Value* pRVal, const std::string& varName = "");

			llvm::Value* Cmp(const ComparisonType type, llvm::Value* pLVal, llvm::Value* pRVal);

			std::unique_ptr<llvm::Module> AddModule(const std::string& name);

			llvm::Function* DeclareFunction(llvm::Module* pModule, const std::string& name, const ValueType returnType, const ValueTypeList* pArgs);
			llvm::Function* DeclareFunction(llvm::Module* pModule, const std::string& name, const ValueType returnType, const NamedValueTypeList& args);
			llvm::Function* DeclareFunction(llvm::Module* pModule, const std::string& name, llvm::FunctionType* type);

			llvm::Function* Function(llvm::Module* pModule, const std::string& name, const ValueType returnType, llvm::Function::LinkageTypes linkage, const ValueTypeList* pArgs);
			llvm::Function* Function(llvm::Module* pModule, const std::string& name, const ValueType returnType, llvm::Function::LinkageTypes linkage, const NamedValueTypeList& args);

			llvm::BasicBlock* Block(llvm::Function* pfn, const std::string& label);
			llvm::BasicBlock* BlockAfter(llvm::Function* pfn, llvm::BasicBlock* pPrevBlock, const std::string& label);

			// This block is not part of any function yet. You'll need to insert it manually
			llvm::BasicBlock* Block(const std::string& label);
			llvm::BasicBlock* CurrentBlock()
			{
				return _builder.GetInsertBlock();
			}
			void SetCurrentBlock(llvm::BasicBlock* pBlock);

			llvm::CallInst* Call(llvm::Function* pfn);
			llvm::CallInst* Call(llvm::Function* pfn, llvm::Value* pArg);
			llvm::CallInst* Call(llvm::Function* pfn, const IRValueList& args);

			llvm::PHINode* Phi(const ValueType type, llvm::Value* pLVal, llvm::BasicBlock* plBlock, llvm::Value* pRVal, llvm::BasicBlock* prBlock);

			llvm::Value* PtrOffset(llvm::Value* pArray, llvm::Value* pOffset);
			llvm::Value* GlobalPtrOffset(llvm::GlobalVariable* pArray, llvm::Value* pOffset);

			llvm::LoadInst* Load(llvm::Value* pPtr);
			llvm::StoreInst* Store(llvm::Value* pPtr, llvm::Value* pVal);

			llvm::AllocaInst* Variable(const ValueType type);
			llvm::AllocaInst* Variable(const ValueType type, const std::string& name);
			llvm::AllocaInst* Variable(llvm::Type* pType, const std::string& name);

			llvm::AllocaInst* StackAlloc(const ValueType type, int count);

			llvm::BranchInst* Branch(llvm::Value* pCondVal, llvm::BasicBlock* pThenBlock, llvm::BasicBlock* pElseBlock);
			llvm::BranchInst* Branch(llvm::BasicBlock* pDest);

			llvm::LLVMContext& Context()
			{
				return _context;
			}

		private:
			llvm::Type* GetValueType(const ValueType type);
			int SizeOf(const ValueType type);
			llvm::Constant* Integer(const ValueType type, const uint64_t value);
			void BindArgTypes(const ValueTypeList& args);
			void BindArgTypes(const NamedValueTypeList& args);
			void BindArgNames(llvm::Function* pfn, const NamedValueTypeList& args);
			llvm::Function* CreateFunction(llvm::Module* pModule, const std::string& name, llvm::Function::LinkageTypes linkage, llvm::FunctionType* pTypeDef);

		private:
			llvm::LLVMContext _context;
			llvm::IRBuilder<> _builder;
			IRVariableTable _stringLiterals;
			// Reusable buffers
			std::vector<llvm::Type *> _argTypes;
			llvm::Value* _derefArgs[2];
			llvm::Value* _pZeroLiteral = nullptr;
		};
	}
}