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

        // create prefab from given game object, does not save it.
        static RscHandle<Prefab> Create(Handle<GameObject> go);

        // save given game object into a prefab, writes into save_path.
        static RscHandle<Prefab> Save(Handle<GameObject> go, string_view save_path);
        static RscHandle<Prefab> SaveAndConnect(Handle<GameObject> go, string_view save_path);

        // get the root of a prefab instance.
        // if the given game object is not part of a prefab instance, returns a null handle.
        static Handle<GameObject> GetPrefabInstanceRoot(Handle<GameObject> go);

		// manually propagate ALL changes of a prefab.
        static void PropagatePrefabChangesToInstances(RscHandle<Prefab> prefab);

        // propagate specific property
        static void PropagatePropertyToInstances(RscHandle<Prefab> prefab, int object_index, string_view component_name, string_view property_path, int component_nth);

        // add component and propagate
        static void AddComponentToPrefab(RscHandle<Prefab> prefab, int object_index, reflect::dynamic component);

        // remove component and propagate
        static void RemoveComponentFromPrefab(RscHandle<Prefab> prefab, int object_index, int component_index);

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
        static void ApplyRemovedComponent(Handle<GameObject> target, string_view component_name, int component_nth);
		static void ApplyPropertyOverride(Handle<GameObject> target, const PropertyOverride& override);
		static void ApplyPrefabInstance(Handle<GameObject> instance_root);
    };
}