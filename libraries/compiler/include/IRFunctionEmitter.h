#pragma once

#include "IREmitter.h"
#include "IRLoopEmitter.h"
#include "IRIfEmitter.h"

namespace emll
{
	namespace compiler
	{
		class IRFunctionEmitter
		{
		public:
			IRFunctionEmitter(void) {}
			IRFunctionEmitter(IREmitter* pEmitter, llvm::Function* pfn);
			IRFunctionEmitter(const IRFunctionEmitter& src);

			llvm::Value* Literal(int value)
			{
				return _pEmitter->Literal(value);
			}
			llvm::Value* Literal(int64_t value)
			{
				return _pEmitter->Literal(value);
			}
			llvm::Value* Literal(double value)
			{
				return _pEmitter->Literal(value);
			}
			llvm::Value* Literal(const std::string& value)
			{
				return _pEmitter->Literal(value);
			}
			llvm::Value* Arg(llvm::ilist_iterator<llvm::Argument>& arg)
			{
				return _pEmitter->Load(&(*arg));
			}
			llvm::Value* Cast(llvm::Value* pValue, ValueType destType)
			{
				return _pEmitter->Cast(pValue, destType);
			}
			llvm::Value* CastFloatToInt(llvm::Value* pValue)
			{
				return _pEmitter->CastFloat(pValue, ValueType::Int32);
			}
			llvm::BasicBlock* Block(const std::string& label)
			{
				return _pEmitter->Block(_pfn, label);
			}
			llvm::BasicBlock* BlockAfter(llvm::BasicBlock* pPrevBlock, const std::string& label)
			{
				return _pEmitter->BlockAfter(_pfn, pPrevBlock, label);
			}
			void AddBlock(llvm::BasicBlock* pBlock);

			llvm::BasicBlock* CurrentBlock()
			{
				return _pEmitter->CurrentBlock();
			}
				
			// Set the block that subsequent code will go into. 
			// Returns the previous block
			llvm::BasicBlock* CurrentBlock(llvm::BasicBlock* pBlock);
			llvm::Value* Call(const std::string& name, llvm::Value* pArg = nullptr);
			llvm::Value* Call(const std::string& name, IRValueList& args);
			llvm::Value* Call(const std::string& name, std::initializer_list<llvm::Value*> args);
			void Ret()
			{
				_pEmitter->ReturnVoid();
			}
			llvm::Value* Ret(llvm::Value* value)
			{
				return _pEmitter->Return(value);
			}

			llvm::Value* Op(OperatorType type, llvm::Value* pLVal, llvm::Value* pRVal)
			{
				return _pEmitter->BinaryOp(type, pLVal, pRVal);
			}
			llvm::Value* Op(OperatorType type, llvm::iterator_range<llvm::Function::arg_iterator>& args);

			void Branch(llvm::BasicBlock* pDestBlock)
			{
				_pEmitter->Branch(pDestBlock);
			}
			void Branch(llvm::Value* pCondVal, llvm::BasicBlock* pThenBlock, llvm::BasicBlock* pElseBlock)
			{
				_pEmitter->Branch(pCondVal, pThenBlock, pElseBlock);
			}
			void Branch(ComparisonType comparision, llvm::Value* pValue, llvm::Value* pTestValue, llvm::BasicBlock* pThenBlock, llvm::BasicBlock* pElseBlock);

			llvm::Value* Cmp(ComparisonType type, llvm::Value* pValue, llvm::Value* pTestValue)
			{
				return _pEmitter->Cmp(type, pValue, pTestValue);
			}
			//------------------------------------------
			//
			// Variables
			//
			//------------------------------------------

			llvm::iterator_range<llvm::Function::arg_iterator> Args()
			{
				return _pfn->args();
			}
			llvm::Argument& FirstArg()
			{
				return *(Args().begin());
			}
			llvm::Value* Var(ValueType type)
			{
				return _pEmitter->Variable(type);
			}
			llvm::Value* Var(ValueType type, const std::string& name)
			{
				return _pEmitter->Variable(type, name);
			}

			llvm::Value* Var(ValueType type, int count)
			{
				return _pEmitter->StackAlloc(type, count);
			}

			llvm::Value* Load(llvm::Value* pPtr)
			{
				return _pEmitter->Load(pPtr);
			}
			llvm::Value* Store(llvm::Value* pPtr, llvm::Value* pValue);
			llvm::Value* OpAndUpdate(llvm::Value* pPtr, OperatorType op, llvm::Value* pValue);

			llvm::Value* PtrOffsetA(llvm::Value* pPtr, int offset);
			llvm::Value* ValueAtA(llvm::Value* pPtr, int offset);
			llvm::Value* SetValueAtA(llvm::Value* pPtr, int offset, llvm::Value* pValue);

			llvm::Value* PtrOffsetH(llvm::Value* pPtr, int offset);
			llvm::Value* PtrOffsetH(llvm::Value* pPtr, llvm::Value* pOffset);
			llvm::Value* ValueAtH(llvm::Value* pPtr, int offset);
			llvm::Value* ValueAtH(llvm::Value* pPtr, llvm::Value* pOffset);
			llvm::Value* SetValueAtH(llvm::Value* pPtr, int offset, llvm::Value* pValue);

			llvm::Value* PtrOffset(llvm::GlobalVariable* pPtr, llvm::Value* pOffset);
			llvm::Value* ValueAt(llvm::GlobalVariable* pGlobal, llvm::Value* pOffset);
			llvm::Value* SetValueAt(llvm::GlobalVariable* pGlobal, llvm::Value* pOffset, llvm::Value* pVal);

			//------------------------------------------
			//
			// Expressions
			//
			//------------------------------------------
			IRForLoopEmitter ForLoop();
			IRIfEmitter If();
			//------------------------------------------
			//
			// Standard function calls
			//
			//------------------------------------------

			llvm::Value* Malloc(ValueType type, int64_t size);
			void Free(llvm::Value* pValue);

			llvm::Value* Print(const std::string& text);
			llvm::Value* Printf(std::initializer_list<llvm::Value*> args);

			void Verify()
			{
				llvm::verifyFunction(*_pfn);
			}

		private:
			llvm::Module* Module()
			{
				return _pfn->getParent();
			}
			llvm::Function* ResolveFunction(const std::string& name);

		private:
			llvm::Function* _pfn = nullptr;
			IREmitter* _pEmitter = nullptr;
			IRValueList _values;
		};
	}
}

