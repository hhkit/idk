#pragma once

#include <core/ISystem.h>

namespace idk
{
    template<typename ConfigT>
    class ConfigurableSystem : public ISystem
    {
    public:
        virtual void Init() = 0;
        virtual void Shutdown() = 0;

        using Config = ConfigT;
        void SetConfig(const Config& config) { ApplyConfig(_config = config); }
        const Config& GetConfig() const { return _config; }

    protected:
        virtual void ApplyConfig(const Config& config) = 0;

    private:
        Config _config;
    };
}