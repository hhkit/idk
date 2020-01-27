#pragma once

#include <core/GameObject.h>
#include <res/GenericResourceHandle.h>

namespace idk
{
    struct ObjectSelection
    {
        vector<Handle<GameObject>> game_objects;
        vector<GenericResourceHandle> assets;

        bool operator==(const ObjectSelection& other) const { return game_objects == other.game_objects && assets == other.assets; }
        bool empty() const { return game_objects.empty() && assets.empty(); }
    };
}