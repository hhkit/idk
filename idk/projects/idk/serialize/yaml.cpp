#include "stdafx.h"
#include "yaml.h"

namespace idk::yaml
{

    type node::type() const
    {
        return yaml::type(_value.index());
    }

	bool node::null() const
	{
		return type() == type::null;
	}

	scalar_type& node::as_scalar()
    {
        return std::get<static_cast<int>(type::scalar)>(_value);
    }

    sequence_type& node::as_sequence()
    {
        return std::get<static_cast<int>(type::sequence)>(_value);
    }

    mapping_type& node::as_mapping()
    {
        return std::get<static_cast<int>(type::mapping)>(_value);
    }

    void node::push_back(const node& node)
    {
        if (type() == type::null)
            _value = sequence_type();
        as_sequence().push_back(node);
    }

    node& node::operator[](size_t index)
    {
        if (type() == type::null)
            _value = sequence_type();
        return as_sequence()[index];
    }

    node& node::operator[](const scalar_type& str)
    {
        if (type() == type::null)
            _value = mapping_type();
        return as_mapping()[str];
    }
}
