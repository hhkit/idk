#include "stdafx.h"
#include "SteamManager.h"
#include <core/Core.inl>
#include <editor/IEditor.h>

#include <steam/steam_api.h>

namespace idk
{
    void SteamManager::Init()
    {
        auto* editor = &Core::GetSystem<IEditor>();
        if (!editor)
        {
            SteamInit();
        }
    }

    void SteamManager::Shutdown()
    {
        SteamShutdown();
    }

    bool SteamManager::SteamInit()
    {
        return _init = SteamAPI_Init();
    }

    void SteamManager::SteamShutdown()
    {
        if (_init)
        {
            SteamAPI_Shutdown();
            _init = false;
        }
    }

    void SteamManager::RunCallbacks()
    {
        SteamAPI_RunCallbacks();
    }

}