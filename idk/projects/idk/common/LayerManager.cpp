#include "stdafx.h"
#include "LayerManager.h"

namespace idk
{
    string_view LayerManager::LayerIndexToName(layer_t layer) const
    {
        if (layer >= num_layers)
            return "";
        return GetConfig().layers[layer];
    }

    LayerManager::layer_t LayerManager::NameToLayerIndex(string_view layer_name) const
    {
        for (layer_t i = 0; i < num_layers; ++i)
        {
            if (GetConfig().layers[i] == layer_name)
                return i;
        }
        return -1;
    }

	LayerMask LayerManager::GetMask(span<string_view> layers) const
    {
		LayerMask mask{ 0 };
        for (auto sv : layers)
        {
            auto index = NameToLayerIndex(sv);
            if (index >= 0)
				mask |= LayerMask{ 1 << index };
        }
        return mask;
    }



    void LayerManager::ApplyConfig(Config& config)
    {
        config.layers[0] = "Default";
        config.layers[1] = "Ignore Raycast";
    }

}