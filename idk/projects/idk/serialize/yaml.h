#pragma once

#include <idk.h>
#include <serialize/serialize.h>

// serializer/parser for a small subset of yaml.
// only supports: blocks, flows, tags, comments
// because of the minimal support,
// we can do a simple top-down one-pass for parsing
namespace idk::yaml
{
    class node;
    using null_type = std::monostate;
    using scalar_type = string;
    using sequence_type = vector<node>;
    using mapping_type = hash_table<scalar_type, node>;

    enum class type : char
    {
        null = 0,
        scalar,
        sequence,
        mapping
    };

	class node
	{
    public:
        node() = default;

        template<typename T, typename = sfinae<is_container_v<T> || is_basic_serializable_v<T>>>
        explicit node(T&& arg);

		type type() const;
		bool null() const;
        template<typename T> T& get();

        scalar_type&    as_scalar();
        sequence_type&  as_sequence();
        mapping_type&   as_mapping();

        void push_back(const node& node);

        template<typename... T>
        node& emplace_back(T&&... args);

        node& operator[](size_t index);
        node& operator[](const scalar_type& str);

    private:
        variant<null_type, scalar_type, sequence_type, mapping_type> _value;
        scalar_type _tag;
	};
     
    node parse(string_view str);
}

#include "yaml.inl"