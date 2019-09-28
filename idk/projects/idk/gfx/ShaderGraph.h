#pragma once

#include "ShaderGraph_data.h"
#include <gfx/Material.h>

namespace idk::shadergraph
{
    class Graph
        : public Material
    {
    public:
        Guid master_node;
        hash_table<Guid, Node> nodes;
        vector<Link> links;
        vector<Parameter> parameters;

        void Compile();
    };
}