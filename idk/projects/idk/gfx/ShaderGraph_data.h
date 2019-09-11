#pragma once

#include <idk.h>
#include <util/enum.h>

namespace idk::shadergraph
{

    ENUM(ValueType, char, FLOAT = 1, VEC2, VEC3, VEC4, MAT2, MAT3, MAT4, SAMPLER2D)

    struct Slot
    {
        ValueType type;
    };

    struct Value
    {
        ValueType type;
        string value;
        Guid node;
        int slot;
    };

    struct Node
    {
        string name;
        Guid guid;
        vec2 position;
        bool selected;
        vector<Slot> input_slots;
        vector<Slot> output_slots;
    };

    struct Link
    {
        Guid node_out;
        Guid node_in;
        int slot_out;
        int slot_in;
    };

    struct NodeSignature
    {
        vector<ValueType> ins;
        vector<ValueType> outs;
        NodeSignature(string str);
    };

    struct NodeTemplate
    {
        vector<NodeSignature> signatures;
        vector<string> names;
        string code;

        static NodeTemplate Parse(std::string filename);
        static hash_table<string, NodeTemplate>& GetMap();

        size_t GetSlotIndex(string_view name) const;
    };

}