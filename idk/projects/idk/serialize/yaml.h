#pragma once

#include <idk.h>
#include <serialize/serialize.h>

// serializer/parser for a small subset of yaml.
// only supports: blocks, flows (only single line tested), local primary tags, comments
// because of the minimal support, we can do a simple top-down one-pass for parsing.
namespace idk::yaml
{
    class node;

    node load(string_view str);
    string dump(const node& node);

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
        template<typename T, typename = sfinae<
            !std::is_same_v<std::decay_t<T>, node> &&
            (is_container_v<T> || is_basic_serializable_v<T>)>>
        explicit node(T&& arg);

        // accessors
		type type() const;
        size_t size() const;
        bool has_tag() const;
        const scalar_type& tag() const;
        const node& at(const scalar_type& key) const;
        template<typename T> T get() const;

        bool is_null() const;
        bool is_scalar() const;
        bool is_sequence() const;
        bool is_mapping() const;

        scalar_type&            as_scalar();
        const scalar_type&      as_scalar() const;
        sequence_type&          as_sequence();
        const sequence_type&    as_sequence() const;
        mapping_type&           as_mapping();
        const mapping_type&     as_mapping() const;

        // modifiers
        void tag(string_view new_tag);
        void push_back(const node& node);
        template<typename... Ts> node& emplace_back(Ts&&... args);
        template<typename... Ts> std::pair<mapping_type::iterator, bool> emplace(Ts&&... args);

        // sequence iterators
        sequence_type::const_iterator begin() const;
        sequence_type::const_iterator end() const;

        node& operator[](size_t index);
        node& operator[](const scalar_type& str);

        template<typename T, typename = sfinae<
            !std::is_same_v<std::decay_t<T>, node> &&
            (is_container_v<T> || is_basic_serializable_v<T>)>>
        node& operator=(T&& val);

    private:
        variant<null_type, scalar_type, sequence_type, mapping_type> _value;
        scalar_type _tag;
	};
}

#include "yaml.inl"