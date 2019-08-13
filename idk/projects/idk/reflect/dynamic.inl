#pragma once

#include <reflect/reflect.h>

namespace idk::reflect
{

	struct dynamic::property_iterator
	{
		property_iterator(const dynamic& obj, size_t index = 0);
		property_iterator& operator++(); //prefix increment
		bool operator==(const property_iterator&);
		bool operator!=(const property_iterator&);
		property operator*() const;

	private:
		const dynamic& obj;
		size_t index;
	};

	template<typename T, typename>
	dynamic::dynamic(T&& obj)
		: type{ get_type<T>() }, _ptr{ std::make_shared<detail::dynamic_derived<T>>(std::forward<T>(obj)) }
	{}

	template<typename T, typename>
	dynamic& dynamic::operator=(const T& rhs)
	{
		assert(is<std::decay_t<T>>());
		type._context->copy_assign(_ptr->get(), &rhs);
		return *this;
	}

	template<typename T>
	bool dynamic::is() const
	{
		return type.hash() == typehash<T>();
	}

	template<typename T>
	T& dynamic::get() const
	{
		return *static_cast<T*>(_ptr->get());
	}

	// recursively visit all members
	// visitor must be a function with signature:
	//  (const char* name, auto&& data, int depth_change) -> bool/void
	// name: name of property
	// data: the value, use T = std::decay_t<decltype(data)> to get the type
	// depth_change: the change in depth; -1, 0, or 1. (1 means down a level)
	// return false to stop recursion. if function doesn't return, it always recurses
	template<typename Visitor>
	void dynamic::visit(Visitor&& visitor) const
	{
		int depth = 0;
		detail::visit(_ptr->get(), type, std::forward<Visitor>(visitor), depth);
	}

}