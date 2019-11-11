#pragma once

#include <core/ISystem.h>

namespace idk
{
    template<typename SystemT, typename = void>
    struct is_configurable_system : std::false_type {};
    template<typename SystemT>
    struct is_configurable_system<SystemT, std::void_t<typename SystemT::Config>> : std::true_type {};


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
        virtual void ApplyConfig(Config& config) = 0;

    private:
        Config _config;
    };
}