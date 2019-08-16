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
	//  (auto&& key, auto&& value, int depth_change) -> bool/void
	// 
	// key:
	//     name of property (const char*), or
	//     container key when visiting container elements ( K = std::decay_t<decltype(key)> )
	//     for sequential containers, it will be size_t. for associative, it will be type K
	// value:
	//     the value, use T = std::decay_t<decltype(value)> to get the type
	// depth_change: (int)
	//     change in depth. -1 (up a level), 0 (stay same level), or 1 (down a level)
	// 
	// return false to stop recursion. if function doesn't return, it always recurses
	template<typename Visitor>
	void dynamic::visit(Visitor&& visitor) const
	{
		int depth = 0;
		detail::visit(_ptr->get(), type, std::forward<Visitor>(visitor), depth);
	}

}