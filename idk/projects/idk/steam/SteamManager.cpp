#include "stdafx.h"
#include "SteamManager.h"
#include <core/Core.inl>
#include <editor/IEditor.h>
#pragma warning(push)
#pragma warning(disable:4996)
#include <steam/steam_api.h>
#pragma warning(pop)

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

    bool SteamManager::SteamLoggedOn()
    {
        return _init && SteamUser()->BLoggedOn();
    }

}