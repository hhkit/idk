#pragma once
#include <idk.h>
#include <file/PathHandle.h>
#include <res/ResourceFactory.h>
#include <gfx/ShaderGraph.h>

namespace idk::shadergraph
{
    class Factory
        : public ResourceFactory<Graph>
    {
    public:
        unique_ptr<Graph> GenerateDefaultResource();
        unique_ptr<Graph> Create();
        unique_ptr<Graph> Create(PathHandle file_handle);
    };
}