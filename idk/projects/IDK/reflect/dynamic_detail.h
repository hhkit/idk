#pragma once

namespace idk::reflect::detail
{

	struct dynamic_base
	{
		virtual void* get() const = 0;
		virtual ~dynamic_base() {}
	};

	template<typename T>
	struct dynamic_derived : dynamic_base
	{
		T obj;

		template<typename U>
		dynamic_derived(U&& obj) : obj{ std::forward<U>(obj) } {}
		void* get() const override { return const_cast<void*>(static_cast<const void*>(&obj)); }
	};

}