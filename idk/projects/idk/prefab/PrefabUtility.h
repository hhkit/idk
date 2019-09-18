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
        static void Save(Handle<GameObject> go, PathHandle save_path);

        // get the root of a prefab instance.
        // if the given game object is not part of a prefab instance, returns a null handle.
        static Handle<GameObject> GetPrefabInstanceRoot(Handle<GameObject> go);

		// manually propagate ALL changes of a prefab.
		// most likely don't need to call this.
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

		static void ApplyAddedComponent(Handle<GameObject> target, GenericHandle component);
        static void ApplyRemovedComponent(Handle<GameObject> target, string_view component_name, int component_add_index);
		static void ApplyPropertyOverride(Handle<GameObject> instance_root, const PropertyOverride& override);
		static void ApplyPrefabInstance(Handle<GameObject> instance_root);
    };
}