#pragma once

#include <core/ConfigurableSystem.h>
#include <common/Layer.h>

namespace idk
{
    struct LayerManagerConfig
    {
        array<string, 32> layers{ "Default", "Ignore Raycast", "UI" };
    };

    class LayerManager : public ConfigurableSystem<LayerManagerConfig>
    {
    public:
        using layer_t = decltype(Layer::index);
        constexpr static size_t num_layers = std::tuple_size_v<decltype(LayerManagerConfig::layers)>;
        constexpr static size_t num_builtin_layers = 3;
        constexpr static layer_t layer_default = 0;
        constexpr static layer_t layer_ignore_raycast = 1;
        constexpr static layer_t layer_ui = 2;

        virtual void Init() {};
        virtual void Shutdown() {};

        string_view LayerIndexToName(layer_t layer) const;
        layer_t NameToLayerIndex(string_view layer_name) const;
        uint32_t GetMask(span<string_view> layers) const;

    protected:
        virtual void ApplyConfig(Config& config);
    };
}