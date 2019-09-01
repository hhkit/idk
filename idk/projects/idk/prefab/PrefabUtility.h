#pragma once

#include <idk.h>
#include <core/Handle.h>

namespace idk
{
    class PrefabUtility
    {
    public:
        // instantiate given prefab in the given scene.
        // for simplicity's sake only prefab roots have PrefabInstance component attached.
        static Handle<GameObject> Instantiate(RscHandle<Prefab> prefab, Scene& scene);

        // save given game object into a prefab, writes into save_path.
        static void Save(Handle<GameObject> go, FileHandle save_path);
    };
}