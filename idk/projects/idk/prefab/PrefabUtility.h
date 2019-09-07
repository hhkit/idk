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

        // get the root of a prefab instance.
        // if the given game object is not part of a prefab instance, returns a null handle.
        static Handle<GameObject> GetPrefabInstanceRoot(Handle<GameObject> go);

        static void RecordPrefabInstanceChange(Handle<GameObject> target, GenericHandle component, string_view property_path);

        static void RevertPropertyOverride(Handle<GameObject> instance_root, const PropertyOverride& override);

        // reverts all overrides of the given prefab instance.
        static void RevertPrefabInstance(Handle<GameObject> instance_root);
    };
}