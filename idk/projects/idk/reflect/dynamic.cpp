#include "stdafx.h"
#include <reflect/reflect.h>

namespace idk::reflect
{
	dynamic::dynamic(reflect::type type, void* obj)
		: type{ type }, _ptr{ std::make_shared<derived<void*>>(std::forward<void*>(obj)) }
	{}

	dynamic::dynamic()
		: type{ nullptr }, _ptr{ nullptr }
	{}

	dynamic& dynamic::operator=(const dynamic& rhs)
	{
		type._context->variant_assign(_ptr->get(), rhs.type._context->get_mega_variant(rhs._ptr->get()));
		return *this;
	}

	bool dynamic::valid() const
	{
		return type.valid();
	}

	dynamic::property_iterator dynamic::begin() const
	{
		return property_iterator(*this);
	}

	dynamic::property_iterator dynamic::end() const
	{
		return property_iterator(*this, this->type._context->table.m_Count);
	}

	property dynamic::get_property(string_view name) const
	{
		auto& table = type._context->table;
		for (size_t i = 0; i < table.m_Count; ++i)
		{
			if (table.m_pEntry[i].m_pName == name)
				return get_property(i);
		}
		throw "Property not found!";
	}

	property dynamic::get_property(size_t index) const
	{
		assert(index <= type._context->table.m_Count);

		auto& entry = type._context->table.m_pActionEntries[index];
		const char* name;
		dynamic val;

		std::visit([&](auto&& fn_getset) {

			using fn_getset_t = std::decay_t<decltype(fn_getset)>;
			using T = ::property::vartype_from_functiongetset<fn_getset_t>;
			void* offsetted = ::property::details::HandleBasePointer(_ptr->get(), entry.m_Offset);
			T& value = *reinterpret_cast<T*>(offsetted);
			new (&val) dynamic{ value };
			name = type._context->table.m_pEntry[index].m_pName;

		}, entry.m_FunctionTypeGetSet);

		return { name, val };
	}

	uni_container dynamic::to_container() const
	{
		return type._context->to_container(_ptr->get());
	}

	enum_value dynamic::to_enum_value() const
	{
		return type._context->to_enum_value(_ptr->get());
	}

	vector<dynamic> dynamic::unpack() const
	{
		return type._context->unpack(_ptr->get());
	}



	dynamic::property_iterator::property_iterator(const dynamic& obj, size_t index)
		: obj{ obj }, index { index }
	{}

	dynamic::property_iterator& dynamic::property_iterator::operator++()
	{
		++index;
		return *this;
	}

	bool dynamic::property_iterator::operator==(const property_iterator& other)
	{
		return index == other.index && obj._ptr == other.obj._ptr;
	}

	bool dynamic::property_iterator::operator!=(const property_iterator& other)
	{
		return index != other.index || obj._ptr != other.obj._ptr;
	}

	property dynamic::property_iterator::operator*() const
	{
		return obj.get_property(index);
	}

}