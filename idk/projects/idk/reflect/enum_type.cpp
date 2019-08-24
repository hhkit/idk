#include "stdafx.h"

#include <reflect/reflect.h>

namespace idk::reflect
{
	enum_type::enum_type(const data* data)
		: _data{ data }
	{
	}

	type enum_type::underlying_type() const
	{
		switch (_data->value_sizeof)
		{
			case sizeof(int8_t) : return get_type<int8_t>();
			case sizeof(int16_t): return get_type<int16_t>();
			case sizeof(int32_t): return get_type<int32_t>();
			case sizeof(int64_t): return get_type<int64_t>();
			default: throw "???";
		}
	}

	enum_value enum_type::from_string(string_view name) const
	{
		for (size_t i = 0; i < _data->count; ++i)
		{
			if (_data->names[i] == name)
				return enum_value{ *this, at(i) };
		}
		return enum_value{ *this, at(0) };
	}

	enum_value enum_type::from_value(int64_t value) const
	{
		return enum_value{ *this, value };
	}

	size_t enum_type::count() const
	{
		return _data->count;
	}

	enum_type::iterator enum_type::begin() const
	{
		return iterator{ *this, 0 };
	}

	enum_type::iterator enum_type::end() const
	{
		return iterator{ *this, _data->count };
	}

	int64_t enum_type::at(size_t index) const
	{
		switch (_data->value_sizeof)
		{
		case sizeof(int8_t) : return r_cast<const int8_t* >(_data->values)[index];
		case sizeof(int16_t): return r_cast<const int16_t*>(_data->values)[index];
		case sizeof(int32_t): return r_cast<const int32_t*>(_data->values)[index];
		case sizeof(int64_t): return r_cast<const int64_t*>(_data->values)[index];
		default: throw "???";
		}
	}

	bool enum_type::operator==(const enum_type& other) const
	{
		return _data == other._data;
	}



	enum_type::iterator::iterator(const enum_type& obj, size_t index)
		: obj{ obj }, index{ index }
	{}

	enum_type::iterator& enum_type::iterator::operator++()
	{
		++index;
		return *this;
	}

	bool enum_type::iterator::operator==(const iterator& other)
	{
		return index == other.index && obj == other.obj;
	}

	bool enum_type::iterator::operator!=(const iterator& other)
	{
		return index != other.index || !(obj == other.obj);
	}

	std::pair<string_view, int64_t> enum_type::iterator::operator*() const
	{
		assert(index < obj.count());
		return std::make_pair(obj._data->names[index], obj.at(index));
	}

}