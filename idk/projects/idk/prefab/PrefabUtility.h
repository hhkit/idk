#pragma once

#include <idk.h>
#include <core/Handle.h>
#include <prefab/PropertyOverride.h>
#include <res/ResourceHandle.h>

namespace idk
{
    class PrefabUtility
    {
    public:
        // instantiate given prefab in the given scene.
        // for simplicity's sake only prefab roots have PrefabInstance component attached.
        static Handle<GameObject> Instantiate(RscHandle<Prefab> prefab, Scene& scene, Handle<GameObject> force_handle = {});

        // instantiate given a particular PrefabInstance, really meant for parsing scene.
        static Handle<GameObject> InstantiateSpecific(Handle<GameObject> handle, const PrefabInstance& prefab_inst);

        // create prefab from given game object, does not save it.
		static Prefab CreateResourceManagerHack(Handle<GameObject> go, Guid guid);
        static RscHandle<Prefab> Create(Handle<GameObject> go);
		static RscHandle<Prefab> Create(Handle<GameObject> go, Guid guid);

        // save given game object into a prefab, writes into save_path.
        static RscHandle<Prefab> Save(Handle<GameObject> go, string_view save_path);
        static RscHandle<Prefab> SaveAndConnect(Handle<GameObject> go, string_view save_path);

        // get the root of a prefab instance.
        // if the given game object is not part of a prefab instance, returns a null handle.
        static Handle<GameObject> GetPrefabInstanceRoot(Handle<GameObject> go);

        static void GetPrefabInstanceComponentDiff(Handle<GameObject> target, vector<int>& out_removed, vector<int>& out_added);

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
        static void RecordPrefabInstanceChange(Handle<GameObject> target, GenericHandle component, string_view property_path, reflect::dynamic val = {});
        static void RecordPrefabInstanceChangeForced(
            Handle<GameObject> target, string_view component_name, int component_nth, string_view property_path, reflect::dynamic val);
        static bool RecordPrefabInstanceRemoveComponent(Handle<GameObject> target, string_view component_name, int component_nth);
        static void RecordDefaultOverrides(Handle<GameObject> target);

        static void RevertPropertyOverride(Handle<GameObject> target, const PropertyOverride& override);
        static void RevertRemovedComponent(Handle<GameObject> target, int component_index, GenericHandle force_handle = {});
        static void RevertPrefabInstance(Handle<GameObject> instance_root);

		static void ApplyAddedComponent(Handle<GameObject> target, GenericHandle component);
        static void ApplyRemovedComponent(Handle<GameObject> target, string_view component_name, int component_nth);
		static void ApplyPropertyOverride(Handle<GameObject> target, const PropertyOverride& override);
		static void ApplyPrefabInstance(Handle<GameObject> instance_root);
    };
}