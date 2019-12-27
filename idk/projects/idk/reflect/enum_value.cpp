#include "stdafx.h"
#include <reflect/reflect.inl>

namespace idk::reflect
{
	enum_value::enum_value(const reflect::enum_type type, int64_t val)
		: enum_type{ type }, _value{ val }
	{}

	string_view enum_value::name() const
	{
		const size_t count = enum_type._data->count;
		for (size_t i = 0; i < count; ++i)
		{
			if (enum_type.at(i) == _value)
				return enum_type._data->names[i];
		}

		return "";
	}

	int64_t enum_value::value() const
	{
		return _value;
	}

	size_t enum_value::index() const
	{
		const size_t count = enum_type._data->count;
		for (size_t i = 0; i < count; ++i)
		{
			if (enum_type.at(i) == _value)
				return i;
		}

		return enum_value::npos;
	}

	enum_value& enum_value::try_assign(string_view name)
	{
		auto* names = enum_type._data->names;
		const size_t count = enum_type._data->count;
		for (size_t i = 0; i < count; ++i)
		{
			if (names[i] == name)
			{
				_value = enum_type.at(i);
				return *this;
			}
		}
		return *this;
	}

	enum_value& enum_value::try_assign(size_t index)
	{
		if (index >= enum_type._data->count)
			return *this;

		_value = enum_type.at(index);
		return *this;
	}

	enum_value& enum_value::try_assign(int64_t val)
	{
		const size_t count = enum_type._data->count;
		for (size_t i = 0; i < count; ++i)
		{
			if (enum_type.at(i) == val)
			{
				_value = val;
				return *this;
			}
		}

		return *this;
	}

	enum_value& enum_value::assign(int64_t val)
	{
		_value = val;
		return *this;
	}

	bool enum_value::operator==(const enum_value& other) const
	{
		return _value == other._value && enum_type == other.enum_type;
	}
}