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
		ManagedThunk() = default;
		ManagedThunk(MonoMethod* method);

		template<typename Ret = MonoObject*, typename ... Args>
		Ret Invoke(Args&& ...) const;

		void* get() const { return thunk; }
	private:
		void* thunk{};
	};

	template<typename Ret, typename ...Args>
	inline Ret ManagedThunk::Invoke(Args&& ... args) const
	{
		using FuncType = Ret (__stdcall*)(decltype(box(args))..., MonoObject**);
		const auto invoker = s_cast<FuncType>(thunk);
		MonoObject* exc{};
		auto retval = invoker(box(args)..., &exc);
		if (exc)
		{
			Core::GetSystem<ScriptSystem>().HandleException(exc);
			return Ret{};
		}
		return retval;
	}
}