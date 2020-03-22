#pragma once

#include <idk.h>
#include <util/enum.h>
#include <res/Guid.h>

namespace idk::shadergraph
{

    ENUM(ValueType, char, FLOAT = 1, VEC2, VEC3, VEC4, /*MAT2, MAT3, MAT4,*/ SAMPLER2D, INVALID = 0)

    struct Slot
    {
		ValueType type{};
        string value; // if unconnected
    };

    struct Node
    {
        string name;
        Guid guid;
        vec2 position;
		bool selected{};
        vector<Slot> input_slots;
        vector<Slot> output_slots;
        string control_values; // if any controls, delimited by |
    };

    struct Link
    {
        Guid node_out;
        Guid node_in;
		int slot_out{};
		int slot_in{};
    };

    struct Parameter
    {
        string name;
		ValueType type{};
        string default_value;
    };

    struct NodeSignature
    {
        vector<ValueType> ins;
        vector<ValueType> outs;
        NodeSignature(const string& str);
    };

    struct NodeTemplate
    {
        using table = hash_table<string, NodeTemplate>;

        vector<NodeSignature> signatures;
        vector<string> names;
        string code;

        static NodeTemplate Parse(string_view filename);
        static void LoadTable(string_view dir);
        static const table& GetTable();

        size_t GetSlotIndex(string_view name) const;
    };

}