#include "stdafx.h"
#include "ShaderGraphFactory.h"
#include <util/ioutils.h>
#include <serialize/serialize.h>

namespace idk::shadergraph
{

    unique_ptr<Graph> Factory::GenerateDefaultResource()
    {
        return std::make_unique<Graph>();
    }

    unique_ptr<Graph> Factory::Create()
    {
        return GenerateDefaultResource();
    }

    unique_ptr<Graph> Factory::Create(FileHandle file_handle)
    {
        auto fs = file_handle.Open(FS_PERMISSIONS::READ);
        auto str = stringify(fs);
        auto ptr = GenerateDefaultResource();
        parse_text(str, *ptr.get());
        return std::move(ptr);
    }

}
