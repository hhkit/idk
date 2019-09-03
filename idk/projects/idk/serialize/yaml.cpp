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

    size_t node::size() const
    {
        switch (type())
        {
        case type::sequence: return as_sequence().size();
        case type::mapping: return as_mapping().size();
        default: return 0;
        }
    }

    const scalar_type& node::tag() const
    {
        return _tag;
    }

    bool node::has_tag() const
    {
        return _tag.size();
    }

	scalar_type& node::as_scalar()
    {
        return std::get<static_cast<int>(type::scalar)>(_value);
    }

    const scalar_type& node::as_scalar() const
    {
        return std::get<static_cast<int>(type::scalar)>(_value);
    }

    sequence_type& node::as_sequence()
    {
        return std::get<static_cast<int>(type::sequence)>(_value);
    }

    const sequence_type& node::as_sequence() const
    {
        return std::get<static_cast<int>(type::sequence)>(_value);
    }

    mapping_type& node::as_mapping()
    {
        return std::get<static_cast<int>(type::mapping)>(_value);
    }

    const mapping_type& node::as_mapping() const
    {
        return std::get<static_cast<int>(type::mapping)>(_value);
    }

    void node::tag(string_view new_tag)
    {
        _tag = new_tag;
    }

    void node::push_back(const node& node)
    {
        if (type() == type::null)
            _value = sequence_type();
        as_sequence().push_back(node);
    }

    sequence_type::const_iterator node::begin() const
    {
        return as_sequence().begin();
    }

    sequence_type::const_iterator node::end() const
    {
        return as_sequence().end();
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
