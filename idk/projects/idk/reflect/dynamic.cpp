#include "stdafx.h"
#include <reflect/reflect.inl>
#include <res/MetaBundle.inl>
#include <ds/span.inl>
#include <ds/result.inl>

namespace idk::reflect
{
	dynamic::dynamic(reflect::type type, void* obj)
		: type{ type }, _ptr{ std::make_shared<voidptr>(type, obj) }
	{}

	dynamic::dynamic()
		: type{ nullptr }, _ptr{ nullptr }
	{}

	dynamic& dynamic::operator=(const dynamic& rhs)
	{
        if (type == rhs.type)
            type._context->copy_assign(_ptr->get(), rhs._ptr->get());
        else if (rhs.type._context->in_mega_variant)
            type._context->variant_assign(_ptr->get(), rhs.type._context->get_mega_variant(rhs._ptr->get()));
        else if (type._context->in_mega_variant)
        {
            auto val = copy();
            auto mega_var = type._context->get_mega_variant(val._ptr->get());
            type._context->variant_assign(mega_var, rhs._ptr->get());
            type._context->variant_assign(_ptr->get(), mega_var);
        }
        else
            LOG_ERROR_TO(LogPool::SYS, "Dynamic assignment failed. (%s = %s)", string(type.name()).c_str(), string(rhs.type.name()).c_str());
		return *this;
	}

	bool dynamic::valid() const
	{
		return type.valid();
	}

	dynamic dynamic::copy() const
	{
		return _ptr->copy();
	}

    dynamic& dynamic::swap(dynamic&& other)
    {
        std::swap(_ptr, other._ptr);
        std::swap(const_cast<reflect::type&>(type), const_cast<reflect::type&>(other.type)); // saved!
        return *this;
    }
    dynamic& dynamic::swap(dynamic& other)
    {
        std::swap(_ptr, other._ptr);
        std::swap(const_cast<reflect::type&>(type), const_cast<reflect::type&>(other.type)); // saved!
        return *this;
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

        return { "", dynamic() };
	}

	property dynamic::get_property(size_t index) const
	{
		assert(index <= type._context->table.m_Count);

		auto& entry = type._context->table.m_pActionEntries[index];
        const char* name = "";
		dynamic val;

		std::visit([&](auto&& fn_getset) {

			using fn_getset_t = std::decay_t<decltype(fn_getset)>;
			using T = ::property::vartype_from_functiongetset<fn_getset_t>;
			void* offsetted = ::property::details::HandleBasePointer(_ptr->get(), entry.m_Offset);
			name = type._context->table.m_pEntry[index].m_pName;

			using fn_getsettype = std::decay_t<decltype(fn_getset)>;
			if constexpr (std::is_same_v<fn_getsettype, std::optional<std::tuple<const detail::table&, void*>>(*)(void*, std::uint64_t) noexcept>)
			{
				std::tuple<const detail::table&, void*> tup = fn_getset(offsetted, 0).value();
				auto n = std::get<0>(tup).m_pName;
				void* value = std::get<1>(tup);
				val.swap(dynamic{ get_type(n), value });
			}
			else
			{
				using T = ::property::vartype_from_functiongetset<fn_getset_t>;
				T& value = *reinterpret_cast<T*>(offsetted);
				val.swap(value);
			}

		}, entry.m_FunctionTypeGetSet);

		return { name, val };
	}

    string dynamic::to_string() const
    {
        return type._context->to_string(_ptr->get());
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

	dynamic dynamic::get_variant_value() const
	{
		return type._context->get_variant_value(_ptr->get());
	}

    void dynamic::set_variant_value(const dynamic& val) const
    {
        type._context->set_variant_value(_ptr->get(), val);
    }

    void dynamic::on_parse() const
    {
        return type._context->on_parse(_ptr->get());
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