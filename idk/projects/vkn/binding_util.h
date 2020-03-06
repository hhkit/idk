#pragma once

#include <vkn/utils/utils.inl>

namespace idk::vkn::bindings
{

	template<typename FunctorTest, typename Binder, typename ...Args>
	void BinderForwardFunc(FunctorTest&& functor, Binder&& binder, Args&&... args)
	{
		meta::Applier<Binder, Args...> applier;
		applier(std::forward<FunctorTest>(functor), std::forward<Binder>(binder), std::forward<Args>(args)...);
	}
	template<typename FunctorTest>
	struct BinderForward
	{
		FunctorTest functor;
		template<typename Binder, typename ...Args>
		void operator()(Binder&& binder, Args&&... args)
		{
			BinderForwardFunc(functor, std::forward<Binder>(binder), std::forward<Args>(args)...);
		}

	};





	template<typename Binder, typename ...Args>
	void SetStateTestFunc(Binder&& binder, Args&&... args)
	{
		volatile int a = 0;
		a += 0;
		binder.SetState(std::forward<Args>(args)...);
	}

	struct SetStateTest
	{
		template<typename Binder, typename ...Args, typename = decltype(std::declval<Binder>().SetState(std::declval<Args&&>()...)) >
		void operator()(Binder&& binder, Args&&... args)
		{
			SetStateTestFunc(std::forward<Binder>(binder), std::forward<Args>(args)...);
		}

	};


}