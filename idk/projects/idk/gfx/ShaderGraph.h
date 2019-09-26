#pragma once

#include "ShaderGraph_data.h"
#include <res/SaveableResource.h>
#include <res/Resource.h>

namespace idk
{
    namespace shadergraph { class Graph; }
    RESOURCE_EXTENSION(shadergraph::Graph, ".mat")
}

namespace idk::shadergraph
{
    class Graph
        : public Resource<Graph>
        , public Saveable<Graph, false_type>
    {
    public:
        Guid master_node;
        hash_table<Guid, Node> nodes;
        vector<Link> links;
        vector<Parameter> parameters;

        void Compile();
    };
}