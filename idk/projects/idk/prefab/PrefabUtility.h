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

        static void PropagatePrefabChangesToInstances(RscHandle<Prefab> prefab);

        // after changing a value in a component, call this fn.
        //   target:        target game object / child object of the prefab instance
        //   component:     handle to the component
        //   property_path: path of the property changed, delimited by '/'. eg. "position", or "position/x"
        static void RecordPrefabInstanceChange(Handle<GameObject> target, GenericHandle component, string_view property_path);

        // reverts a single override of the given prefab instance.
        static void RevertPropertyOverride(Handle<GameObject> instance_root, const PropertyOverride& override);

        // reverts all overrides of the given prefab instance.
        static void RevertPrefabInstance(Handle<GameObject> instance_root);
    };
}