#pragma once

#include <core/ISystem.h>

namespace idk
{
    class SteamManager : public ISystem
    {
    public:
        bool SteamInitialized() { return _init; }

        bool SteamInit();
        void SteamShutdown();
        void RunCallbacks();

    private:
        bool _init = false;

        void Init() override;
        void Shutdown() override;
    };
}