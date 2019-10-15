#pragma once
#include <idk.h>

#include <utility>

#include <mono/jit/jit.h>
#include <script/ValueBoxer.h>

namespace idk::mono
{
	class ManagedThunk
	{
	public:
		ManagedThunk(MonoMethod* method);

		template<typename Ret = MonoObject*, typename ... Args>
		Ret Invoke(Args&& ...);

		void* get() const { return thunk; }
	private:
		void* thunk;
	};

	template<typename Ret, typename ...Args>
	inline Ret ManagedThunk::Invoke(Args&& ... args)
	{
		using FuncType = Ret (*)(decltype(box(args))..., MonoException**);
		const auto invoker = s_cast<FuncType>(thunk);
		MonoException* exc;
		return invoker(box(args)..., &exc);
	}

	// c++ interface: void MonoBehavior.OnTriggerEnter(Handle<Collider>);
	//     internal:  MonoObject* (*)(MonoObject* behavior, uint64t collider id);
	// 1. need to cast thunk to function
	// 2. need to marshal objects

	// basic types: marshal by ptr
	// value types: marshal by ptr
	// handles:   create monoobject
	// behaviors: fetch monoobject, invoke
	// c++
	// c#: interface: void MonoBehavior:OnTriggerEnter(Collider other);
	//	   internal:  void MonoBehavior\internal:OnTriggerEnter(ulong id);
	//                  create component from id :D
	//

}