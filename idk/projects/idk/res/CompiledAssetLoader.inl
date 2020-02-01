#pragma once

#include "CompiledAssetLoader.h"
#include <res/ResourceManager.inl>
#include <res/SaveableResource.h>
#include <serialize/binary.inl>
#include <serialize/text.inl>
#include <util/ioutils.h>
#include <meta/tag.inl>

namespace idk
{
    template<typename CompiledAsset, typename EngineResource, bool Binary>
    inline void CompiledAssetLoader<CompiledAsset, EngineResource, Binary>::LoadAsset(PathHandle handle)
    {
        auto res = [&]()
        {
            if constexpr (Binary)
            {
                auto istream = handle.Open(FS_PERMISSIONS::READ, true);
                return parse_binary<CompiledAsset>(binarify(istream));
            }
            else
            {
                auto istream = handle.Open(FS_PERMISSIONS::READ, false);
                return parse_text<CompiledAsset>(stringify(istream));
            }
        }();
        if (res)
        {
            Guid guid{ handle.GetStem() };
            auto ptr = Core::GetResourceManager().LoaderEmplaceResource<EngineResource>(guid, std::move(*res));
            if constexpr (has_tag_v<EngineResource, Saveable>)
                ptr->SetSaveableFlag(false);
        }
    }
}