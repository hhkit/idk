#pragma once
#include "RenderBindings.h"
#include <vkn/utils/utils.inl>

namespace idk::vkn::bindings
{
	namespace detail
	{
		template<template<typename, typename...>typename cond, typename ...FArgs>
		struct for_each_binder_impl
		{
			template<typename T>
			using cond2 = cond<T, FArgs...>;
			template<typename T>
			using cond_bool = meta::SfinaeBool<cond2, T>;
		};

	}
	template<typename ...Args>
	template<template<typename...>typename cond, typename Fn, typename ...FArgs>
	void CombinedBindings<Args...>::for_each_binder(Fn&& f, FArgs& ...args)
	{
		meta::for_each_tuple_element<typename detail::for_each_binder_impl<cond, FArgs...>::cond_bool>(binders,
			[](auto& binder, auto& func, auto& ...args)
			{
				func(binder, args...);
			}
		, f, args...);
	}
	template<typename ...Args>
	template<typename Binding, typename Result>
	Result& CombinedBindings<Args...>::Get()
	{
		return std::get<Binding>(binders);
	}
	template<typename ...Args>
	inline bool CombinedBindings<Args...>::Skip(RenderInterface& the_interface, const RenderObject& dc)
	{
		return (std::get<Args>(binders).Skip(the_interface, dc) | ...);
	}
	template<typename ...Args>
	void CombinedBindings<Args...>::Bind(RenderInterface& the_interface)
	{
		meta::for_each_tuple_element(binders, [](RenderBindings& binder, auto& the_interface)
			{
				binder.Bind(the_interface);
			}, the_interface);
	}
	template<typename ...Args>
	void CombinedBindings<Args...>::Bind(RenderInterface& the_interface, const RenderObject& dc)
	{
		meta::for_each_tuple_element(binders, [](RenderBindings& binder, auto& the_interface, auto& dc) {
			binder.Bind(the_interface, dc);
			}, the_interface, dc);
	}
	template<typename ...Args>
	void CombinedBindings<Args...>::BindAni(RenderInterface& the_interface, const AnimatedRenderObject& dc)
	{
		meta::for_each_tuple_element(binders, [](RenderBindings& binder, auto& the_interface, auto& dc) {
			binder.BindAni(the_interface, dc);
			}, the_interface, dc);
	}
}