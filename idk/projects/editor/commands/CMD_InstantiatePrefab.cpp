#include "pch.h"
#include "CMD_InstantiatePrefab.h"
#include <prefab/PrefabUtility.h>
#include <scene/SceneManager.h>
#include <common/Transform.h>
#include <res/ResourceHandle.inl>
namespace idk
{

    CMD_InstantiatePrefab::CMD_InstantiatePrefab(RscHandle<Prefab> prefab, vec3 pos)
        : _prefab{ prefab }, _pos{ pos }
    {
    }

    bool CMD_InstantiatePrefab::execute()
    {
        _handle = PrefabUtility::Instantiate(_prefab, *Core::GetSystem<SceneManager>().GetActiveScene());
        _handle->Transform()->position = _pos;
        return true;
    }

    bool CMD_InstantiatePrefab::undo()
    {
        GameState::GetGameState().DestroyObject(_handle);
        return true;
    }

}