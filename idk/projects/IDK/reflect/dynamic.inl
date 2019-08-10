#pragma once

#include <reflect/reflect.h>

namespace idk::reflect
{
	template<typename T, typename>
	dynamic::dynamic(T&& obj)
		: type{ get_type<T>() }, _ptr{ std::make_shared<detail::dynamic_derived<T>>(std::forward<T>(obj)) }
	{}

	template<typename T, typename>
	dynamic& dynamic::operator=(const T& rhs)
	{
		assert(this->type.is<std::decay_t<T>>());
		type._context->copy_assign(_ptr->get(), &rhs);
		return *this;
	}

	template<typename T>
	bool dynamic::is() const
	{
		return type.hash() == detail::typehash<T>();
	}

	template<typename T>
	T& dynamic::get()
	{
		return *static_cast<T*>(_ptr->get());
	}

	template<typename Visitor>
	void dynamic::visit(Visitor&& visitor)
	{
		detail::visit(_ptr->get(), type._context->table, std::forward<Visitor>(visitor));
	}
}