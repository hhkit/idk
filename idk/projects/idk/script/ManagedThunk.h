#pragma once
#include <idk.h>

#include <utility>

#include <mono/jit/jit.h>

#include <script/MonoBehavior.h>
#include <script/ManagedObj.h>

namespace idk::mono
{
	class ManagedThunk
	{
	public:
		ManagedThunk(MonoMethod* method);

		template<typename ... Args>
		MonoObject* Invoke(Args&& ...);
	private:
		void* thunk;
	};

	template<typename T>
	struct Boxer 
	{
		using type = std::decay_t<T>;
		type operator()(T& obj) const { return obj; }
	};

	template<typename T>
	struct Boxer<Handle<T>>
	{
		using type = uint64_t;
		type operator()(const Handle<T>& obj) const { return obj.id; }
	};

	template<>
	struct Boxer<Handle<mono::Behavior>>
	{
		using type = MonoObject*;
		type operator()(const Handle<mono::Behavior>& obj) const { return obj->GetMonoObject();  };
	};

	template<> struct Boxer<ManagedObject>
	{
		using type = MonoObject *;
		type operator()(const ManagedObject& obj) const noexcept { return obj.Fetch(); };
	};

	template<typename ...Args>
	inline MonoObject* ManagedThunk::Invoke(Args&& ... args)
	{
		using FuncType = MonoObject * (*)(typename Boxer < std::decay_t<Args> > ::type...);
		const auto invoker = s_cast<FuncType>(thunk);
		return invoker(Boxer<std::decay_t<Args>>{}(args)...);
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