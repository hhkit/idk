#pragma once
#include <idk.h>
#include <file/PathHandle.h>
#include <res/ResourceFactory.h>
#include <gfx/ShaderGraph.h>

namespace idk::shadergraph
{
    class Loader
        : public IFileLoader
    {
    public:
		ResourceBundle LoadFile(PathHandle file_handle);
		ResourceBundle LoadFile(PathHandle file_handle, const MetaBundle& meta);
    };
}