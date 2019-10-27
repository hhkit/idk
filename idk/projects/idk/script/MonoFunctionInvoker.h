#pragma once
#include <mono/jit/jit.h>
#include <script/ManagedThunk.h>
#include <script/ValueBoxer.h>
#include <script/ValueUnboxer.h>

namespace idk::mono
{
	using MethodResult = std::variant <ManagedThunk, MonoMethod*, std::nullopt_t>;
	template<typename Ret = MonoObject*, typename Invokee, typename ... Args>
	Ret Invoke(const MethodResult& fn, Invokee&& invokee, Args&& ... args);


	template<typename Ret, typename Invokee, typename ...Args>
	Ret Invoke(const MethodResult& fn, Invokee&& invokee, Args&& ...args)
	{
		auto retval = std::visit([&](auto&& mono_func) -> MonoObject*
			{
				using MethodType = std::decay_t<decltype(mono_func)>;

				if constexpr (std::is_same_v<MethodType, MonoMethod*>)
				{
				//	void* pass_args[] = { &args..., 0 };
				//	return mono_runtime_invoke(mono_func, box(invokee), pass_args, nullptr);
					return Ret{};
				}
				else
				if constexpr (std::is_same_v<MethodType, ManagedThunk>)
				{
					const ManagedThunk& thunk = mono_func;
					return thunk.Invoke(invokee, args...);
				}
				else
					return Ret{};
			}, fn);
		return retval;
	}
}