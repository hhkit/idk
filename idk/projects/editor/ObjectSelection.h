#pragma once

#include <core/GameObject.h>
#include <res/GenericResourceHandle.h>

namespace idk
{
    struct ObjectSelection
    {
        vector<Handle<GameObject>> game_objects;
        vector<GenericResourceHandle> assets;
    };
}