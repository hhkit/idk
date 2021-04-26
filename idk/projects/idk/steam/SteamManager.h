#pragma once

#include <core/ISystem.h>

namespace idk
{
    class SteamManager : public ISystem
    {
    public:
        bool SteamInit();
        void SteamShutdown();
        void RunCallbacks();

        bool SteamInitialized() { return _init; }
        bool SteamLoggedOn();

    private:
        bool _init = false;

        void Init() override;
        void Shutdown() override;
    };
}