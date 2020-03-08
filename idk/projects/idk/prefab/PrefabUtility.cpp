#include "stdafx.h"
#include "PrefabUtility.h"

#include <ds/small_string.h>
#include <serialize/text.h>
#include <file/FileSystem.h>
#include <reflect/reflect.inl>
#include <core/GameObject.inl>
#include <common/Transform.h>
#include <common/Name.h>
#include <scene/SceneManager.h>
#include <scene/SceneGraph.inl>
#include <ds/slow_tree.inl>
#include <script/MonoBehavior.h>
#include <script/ManagedObj.inl> //Assign
#include <prefab/Prefab.h>
#include <prefab/PrefabInstance.h>
#include <res/ResourceManager.inl>
#include <res/ResourceHandle.inl>
#include <ds/span.inl>
#include <ds/result.inl>
#include <util/property_path.h>

namespace idk
{

    namespace helpers
    {

        static int prefab_data_component_nth_to_instance_component_nth(Handle<GameObject> target, string_view component_name, int component_nth)
        {
            // offset nth by removed components to sync that to prefab data

            auto prefab_inst = target->GetComponent<PrefabInstance>();
            IDK_ASSERT(prefab_inst);

            auto& removed = prefab_inst->removed_components; // sort in ascending nth
            std::sort(removed.begin(), removed.end(), [](const ComponentNth& a, const ComponentNth& b) {
                int diff = a.component_name.compare(b.component_name);
                return diff == 0 ? (a.component_nth < b.component_nth) : (diff < 0);
            });

            for (auto iter = removed.rbegin(); iter != removed.rend(); ++iter)
            {
                if (component_name == iter->component_name && iter->component_nth <= component_nth)
                    --component_nth;
            }

            return component_nth;
        }

        static int instance_component_nth_to_prefab_data_component_nth(Handle<GameObject> target, string_view component_name, int component_nth)
        {
            // offset nth by removed components to sync that to prefab data

            auto prefab_inst = target->GetComponent<PrefabInstance>();
            IDK_ASSERT(prefab_inst);

            auto& removed = prefab_inst->removed_components; // sort in ascending nth
            std::sort(removed.begin(), removed.end(), [](const ComponentNth& a, const ComponentNth& b) {
                int diff = a.component_name.compare(b.component_name);
                return diff == 0 ? (a.component_nth < b.component_nth) : (diff < 0);
            });

            for (const auto& i : removed)
            {
                if (component_name == i.component_name && i.component_nth <= component_nth)
                    ++component_nth;
            }

            return component_nth;
        }

        static int instance_component_to_prefab_data_component_nth(Handle<GameObject> target, GenericHandle component)
        {
            auto prefab_inst = target->GetComponent<PrefabInstance>();
            IDK_ASSERT(prefab_inst);

            const auto type = component.type;
            int nth = 0;
            for (const auto c : target->GetComponents())
            {
                if (c == component)
                    break;
                if (type == c.type)
                    ++nth;
            }

            return instance_component_nth_to_prefab_data_component_nth(target, (*component).type.name(), nth);
        }

        static void add_component(Handle<GameObject> go, const reflect::dynamic& prefab_comp)
        {
            if (prefab_comp.is<Transform>())
            {
                auto& t_prefab = prefab_comp.get<Transform>();
                auto& t_ori = *go->GetComponent<Transform>();
                t_ori.position = t_prefab.position;
                t_ori.rotation = t_prefab.rotation;
                t_ori.scale = t_prefab.scale;
            }
            else if (prefab_comp.is<Name>())
                go->Name(prefab_comp.get<Name>().name);
            else
                go->AddComponent(prefab_comp);
        }

        static GenericHandle get_component(Handle<GameObject> go, string_view component_name, int component_nth)
        {
            if (component_nth == 0)
                return go->GetComponent(component_name);
            else if (component_nth > 0)
            {
                const span comps = go->GetComponents();
                const auto tid = GameState::GetGameState().GetTypeID(reflect::get_type(component_name));
                for (auto& c : comps)
                {
                    if (c.type == tid)
                        return (&c)[component_nth];
                }
            }
            return GenericHandle();
        }

        const static PropertyOverride default_overrides[]{
            { "Transform", "position" }, { "Transform", "rotation" }, { "Transform", "parent" },
            { "Name", "name" }
        };
        constexpr static auto num_default_overrides = std::size(default_overrides);

        static bool is_default_override(const PropertyOverride& ov, bool is_root = true)
        {
            if (is_root)
            {
                if (ov.component_name == "Transform")
                {
                    if (ov.property_path.find("position") == 0 || ov.property_path.find("rotation") == 0 || ov.property_path.find("parent") == 0)
                        return true;
                }
                else if (ov.component_name == "Name")
                    return true;
            }
            else
            {
                if (ov.component_name == "Transform" && ov.property_path.find("parent") == 0)
                    return true;
            }

            return false;
        }

		static int find_override(PrefabInstance& prefab_inst, string_view component_name, string_view property_path, int component_nth)
		{
            int i = -1;
			for (auto& ov : prefab_inst.overrides)
			{
                ++i;

				if (ov.component_name != component_name)
					continue;
                if (ov.component_nth != component_nth)
                    continue;

				if (ov.property_path.size() == property_path.size())
				{
					if (ov.property_path == property_path)
						return i;
				}
				else if (ov.property_path.size() > property_path.size()) // eg. scale/x vs scale
				{
					if (ov.property_path.find(property_path) == 0 && ov.property_path[property_path.size()] == '/')
					{
						ov.property_path = property_path; // make ov the shorter one
						return i;
					}
				}
				else // eg. scale vs scale/x
				{
					if (ov.property_path.find(property_path) == 0 && ov.property_path[property_path.size()] == '/')
						return i;
				}
			}
			return -1;
		}

        static bool has_override(PrefabInstance& prefab_inst, string_view component_name, string_view property_path, int component_nth)
        {
            return find_override(prefab_inst, component_name, property_path, component_nth) >= 0;
        }

        static void add_default_overrides(PrefabInstance& prefab_inst)
        {
            constexpr auto add_ov = [](PrefabInstance& inst, string_view name, string_view path)
            {
                auto ov_i = find_override(inst, name, path, 0);
                if (ov_i >= 0)
                {
                    inst.overrides[ov_i].value.swap(resolve_property_path(
                        *get_component(inst.GetGameObject(), name, 0), path));
                }
                else
                {
                    inst.overrides.push_back({ name, path });
                    inst.overrides.back().value.swap(resolve_property_path(
                        *get_component(inst.GetGameObject(), name, 0), path));
                }
            };

            if (prefab_inst.object_index > 0)
            {
                add_ov(prefab_inst, "Transform", "parent");
            }
            else
            {
                for (const auto& ov : default_overrides)
                    add_ov(prefab_inst, ov.component_name, ov.property_path);
            }
        }


        static void propagate_property(Handle<PrefabInstance> origin, int component_index, string_view property_path)
		{
            auto prefab = origin->prefab;
			for (auto& prefab_inst : GameState::GetGameState().GetObjectsOfType<PrefabInstance>())
			{
                if (prefab_inst.prefab != prefab ||
                    prefab_inst.GetGameObject() == origin->GetGameObject() ||
                    prefab_inst.object_index != origin->object_index)
                    continue;

                auto& comp = prefab->data[prefab_inst.object_index].components[component_index];
                int component_nth = prefab->data[prefab_inst.object_index].GetComponentNth(component_index);
				const auto component_name = comp.type.name();
				if (!has_override(prefab_inst, component_name, property_path, component_nth))
				{
                    if (component_name == "MonoBehavior")
                    {
                        auto mb = handle_cast<mono::Behavior>(helpers::get_component(prefab_inst.GetGameObject(), component_name, component_nth));
                        auto& obj = mb->GetObject();
                        obj = mono::ManagedObject{ comp.get<mono::Behavior>().GetObject() };
                        obj.Assign("handle", mb.id);
                    }
                    else
                        assign_property_path(*helpers::get_component(prefab_inst.GetGameObject(), component_name, component_nth), 
                                             property_path,
                                             resolve_property_path(comp, property_path));
				}
			}
		}

		static void propagate_added_component(Handle<PrefabInstance> origin, int component_index)
		{
            auto prefab = origin->prefab;
			for (auto& prefab_inst : GameState::GetGameState().GetObjectsOfType<PrefabInstance>())
			{
				if (prefab_inst.prefab != prefab ||
                    prefab_inst.GetGameObject() == origin->GetGameObject() ||
                    prefab_inst.object_index != origin->object_index)
					continue;

				prefab_inst.GetGameObject()->AddComponent(
                    prefab->data[prefab_inst.object_index].components[component_index]);
			}
		}
        static void propagate_added_component(RscHandle<Prefab> prefab, int object_index, int component_index)
        {
            for (auto& prefab_inst : GameState::GetGameState().GetObjectsOfType<PrefabInstance>())
            {
                if (prefab_inst.prefab != prefab ||
                    prefab_inst.object_index != object_index)
                    continue;

                prefab_inst.GetGameObject()->AddComponent(
                    prefab->data[prefab_inst.object_index].components[component_index]);
            }
        }

        static void propagate_removed_component(Handle<PrefabInstance> origin, string_view component_name, int component_nth)
        {
            const auto prefab = origin->prefab;
            for (auto& prefab_inst : GameState::GetGameState().GetObjectsOfType<PrefabInstance>())
            {
                if (prefab_inst.prefab != prefab ||
                    prefab_inst.GetGameObject() == origin->GetGameObject() ||
                    prefab_inst.object_index != origin->object_index)
                    continue;

                int nth = prefab_data_component_nth_to_instance_component_nth(prefab_inst.GetGameObject(), component_name, component_nth);
                for (auto c : prefab_inst.GetGameObject()->GetComponents())
                {
                    if ((*c).type.name() == component_name)
                    {
                        if (nth == 0)
                        {
                            prefab_inst.GetGameObject()->RemoveComponent(c);
                            break;
                        }
                        --nth;
                    }
                }
            }
        }
        static void propagate_removed_component(RscHandle<Prefab> prefab, int object_index, string_view component_name, int component_nth)
        {
            const auto type = reflect::get_type(component_name);
            for (auto& prefab_inst : GameState::GetGameState().GetObjectsOfType<PrefabInstance>())
            {
                if (prefab_inst.prefab != prefab ||
                    prefab_inst.object_index != object_index)
                    continue;

                // remove all overrides for the component
                for (auto iter = prefab_inst.overrides.begin(); iter != prefab_inst.overrides.end(); ++iter)
                {
                    if (iter->component_nth == component_nth && iter->component_name == component_name)
                        iter = prefab_inst.overrides.erase(iter);
                }

                bool removed = false;
                // see if it's already removed
                for (auto iter = prefab_inst.removed_components.begin(); iter != prefab_inst.removed_components.end(); ++iter)
                {
                    if (iter->component_nth == component_nth && iter->component_name == component_name)
                    {
                        prefab_inst.removed_components.erase(iter);
                        removed = true;
                        break;
                    }
                }

                if (removed)
                    continue;

                // otherwise find it and kill it
                int nth = prefab_data_component_nth_to_instance_component_nth(prefab_inst.GetGameObject(), component_name, component_nth);
                for (auto c : prefab_inst.GetGameObject()->GetComponents())
                {
                    if ((*c).type == type && nth-- == 0)
                    {
                        prefab_inst.GetGameObject()->RemoveComponent(c);
                        break;
                    }
                }
            }
        }
    }



    Handle<GameObject> PrefabUtility::Instantiate(RscHandle<Prefab> prefab, Scene& scene, Handle<GameObject> force_handle)
    {
        if (!prefab)
            return Handle<GameObject>();

        const auto handle = force_handle != Handle<GameObject>() ? scene.CreateGameObject(force_handle) : scene.CreateGameObject();
        const auto& prefab_data = prefab->data;
        int i = 0;
        for (const auto& d : prefab_data[i].components)
            helpers::add_component(handle, d);
        auto prefab_inst = handle->AddComponent<PrefabInstance>();
        helpers::add_default_overrides(*prefab_inst);
        prefab_inst->prefab = prefab;
        prefab_inst->object_index = i;

        vector<Handle<GameObject>> game_objects{ handle };
        while (++i < prefab_data.size())
        {
            const auto child_handle = scene.CreateGameObject();
            game_objects.push_back(child_handle);

            for (const auto& d : prefab_data[i].components)
                helpers::add_component(child_handle, d);
            child_handle->Transform()->parent = game_objects[prefab_data[i].parent_index];

            prefab_inst = child_handle->AddComponent<PrefabInstance>();
            helpers::add_default_overrides(*prefab_inst);
            prefab_inst->prefab = prefab;
            prefab_inst->object_index = i;
        }

        // root name should be based on prefab asset name/path, not namecomponent
        if (prefab->Name().size())
            handle->Name(prefab->Name());
        else if (auto path = Core::GetResourceManager().GetPath(prefab))
            handle->Name(PathHandle(*path).GetStem());

		for (auto& elem : game_objects)
			Core::GetSystem<SceneManager>().InsertObject(elem);

        return handle;
    }

    Handle<GameObject> PrefabUtility::InstantiateSpecific(Handle<GameObject> handle, const PrefabInstance& prefab_inst)
    {
        const auto& prefab = prefab_inst.prefab;
        if (!prefab || handle.id == 0)
            return {};

        if (prefab_inst.object_index >= prefab->data.size())
            return {};

        const auto& prefab_data = prefab->data[prefab_inst.object_index];

        vector<int> removed;
        for (const auto& nth : prefab_inst.removed_components)
            removed.push_back(prefab_data.GetComponentIndex(nth.component_name, nth.component_nth));

        for (size_t i = 0; i < prefab_data.components.size(); ++i)
        {
            const auto& d = prefab_data.components[i];
            if (std::find(removed.begin(), removed.end(), i) == removed.end()) // not removed
                helpers::add_component(handle, d);
        }

        handle->AddComponent(prefab_inst);

        for (const auto& ov : prefab_inst.overrides)
        {
            const int nth = helpers::prefab_data_component_nth_to_instance_component_nth(handle, ov.component_name, ov.component_nth);
            if (const auto comp = helpers::get_component(handle, ov.component_name, nth))
            {
                assign_property_path(*comp, ov.property_path, ov.value);
            }
        }

        return handle;
    }

    static void _create(Handle<GameObject> go, RscHandle<Prefab> prefab_handle, Prefab& prefab, bool connect_inst = false)
    {
        if (connect_inst)
        {
            auto prefab_inst = go->AddComponent<PrefabInstance>();
            helpers::add_default_overrides(*prefab_inst);
            prefab_inst->prefab = prefab_handle;
            prefab_inst->object_index = 0;
        }

        prefab.data.clear();
        auto& root = prefab.data.emplace_back();
        for (auto& c : go->GetComponents())
        {
            if (c.is_type<PrefabInstance>())
                continue;
            root.components.emplace_back((*c).copy());
            if (c.is_type<Transform>())
                root.components.back().get<Transform>().parent = {};
        }

        // build tree
		Scene scene{ go.scene };

        vector<small_string<GenericHandle::index_t>> nodes;
        vector<GenericHandle::gen_t> gens;
        for (const auto& o : scene)
        {
            const auto index = static_cast<size_t>(o.GetHandle().index);
            if (index >= gens.size())
            {
                nodes.resize(index + 1);
                gens.resize(index + 1);
            }
            gens[index] = o.GetHandle().gen;
            if (const auto parent = o.Parent())
            {
                if (parent.index >= nodes.size())
                {
                    nodes.resize(parent.index + 1);
                    gens.resize(parent.index + 1);
                }
                nodes[o.Parent().index] += static_cast<GenericHandle::index_t>(index);
            }
        }

        // tree walk
        small_string<GenericHandle::index_t> stack(1, go.index);
        small_string<GenericHandle::index_t> game_objects;
        while (stack.size())
        {
            auto curr_par = stack.back();
            stack.pop_back();
            {
                size_t i = nodes[curr_par].size();
				decltype(stack) reversed;
				reversed.reserve(i);
				while (i--) // add in reverse order because we take elems from back
					reversed += nodes[curr_par][i];
				stack = reversed + stack;
            }
            game_objects += curr_par;

            for (auto child_index : nodes[curr_par])
            {
                const Handle<GameObject> child{ child_index, gens[child_index], go.scene };
                PrefabData& child_prefab_data = prefab.data.emplace_back();
                for (auto& c : child->GetComponents())
                {
                    if (c.is_type<PrefabInstance>())
                        continue;
                    child_prefab_data.components.emplace_back((*c).copy());
                }
                child_prefab_data.parent_index = static_cast<int>(game_objects.find(curr_par));

                if (connect_inst)
                {
                    auto prefab_inst = child->AddComponent<PrefabInstance>();
                    helpers::add_default_overrides(*prefab_inst);
                    prefab_inst->prefab = prefab_handle;
                    prefab_inst->object_index = static_cast<int>(prefab.data.size() - 1);
                }
            }
        }

        prefab.Name(go->Name());
    }

    Prefab PrefabUtility::CreateResourceManagerHack(Handle<GameObject> go, Guid guid)
	{
		Prefab v;
		_create(go, RscHandle<Prefab>{guid}, v);
		return v;
	}

	RscHandle<Prefab> PrefabUtility::Create(Handle<GameObject> go)
    {
        auto handle = Core::GetResourceManager().Create<Prefab>();
        _create(go, handle, *handle);
        return handle;
    }

	RscHandle<Prefab> PrefabUtility::Create(Handle<GameObject> go, Guid guid)
	{
		auto handle = Core::GetResourceManager().LoaderCreateResource<Prefab>(guid);
		_create(go, handle, *handle);
		return handle;
	}

	RscHandle<Prefab> PrefabUtility::Save(Handle<GameObject> go, string_view save_path)
    {
        auto handle = [save_path]() {
            auto create_res = Core::GetResourceManager().Create<Prefab>(save_path);
            if (create_res)
                return *create_res;
            else
                return *Core::GetResourceManager().Load<Prefab>(save_path);
        }();

        _create(go, handle, *handle);
        return handle;
    }

    RscHandle<Prefab> PrefabUtility::SaveAndConnect(Handle<GameObject> go, string_view save_path)
    {
        auto handle = [save_path]() {
            auto create_res = Core::GetResourceManager().Create<Prefab>(save_path);
            if (create_res)
                return *create_res;
            else
                return *Core::GetResourceManager().Load<Prefab>(save_path);
        }();

        _create(go, handle, *handle, true);
        return handle;
    }

    Handle<GameObject> PrefabUtility::GetPrefabInstanceRoot(Handle<GameObject> go)
    {
        if (!go->HasComponent<PrefabInstance>())
            return Handle<GameObject>();
        while (go)
        {
            if (const auto inst = go->GetComponent<PrefabInstance>(); inst && inst->object_index == 0)
                return go;
            go = go->Parent();
        }
        return go;
    }

    void PrefabUtility::GetPrefabInstanceComponentDiff(Handle<GameObject> target, vector<int>& out_removed, vector<int>& out_added)
    {
        const auto obj_prefab_inst = target->GetComponent<PrefabInstance>();
        if (!obj_prefab_inst)
            return;

        const auto& prefab = *obj_prefab_inst->prefab;

        auto& prefab_data = prefab.data[obj_prefab_inst->object_index];

        if (obj_prefab_inst->removed_components.empty() && target->GetComponents().size() == prefab_data.components.size())
            return;

        for (const auto& nth : obj_prefab_inst->removed_components)
            out_removed.push_back(prefab_data.GetComponentIndex(nth.component_name, nth.component_nth));

        vector<GenericHandle> obj_component_handles;
        obj_component_handles.reserve(target->GetComponents().size());

        for (auto c : target->GetComponents())
            obj_component_handles.push_back(c);

        for (size_t i = 0; i < prefab_data.components.size(); ++i)
        {
            if (std::find(out_removed.begin(), out_removed.end(), i) != out_removed.end()) // i is removed
                continue;

            const auto tid = GameState::GetGameState().GetTypeID(prefab_data.components[i].type);
            for (size_t j = 0; j < obj_component_handles.size(); ++j)
            {
                if (tid == obj_component_handles[j].type)
                {
                    obj_component_handles[j] = GenericHandle();
                    break;
                }
            }
        }

        // ptrs left in obj comp ptrs == added components
        for (int i = 0; i < obj_component_handles.size(); ++i)
        {
            if (obj_component_handles[i] && !obj_component_handles[i].is_type<PrefabInstance>())
                out_added.push_back(i);
        }
    }

    void PrefabUtility::PropagatePrefabChangesToInstances(RscHandle<Prefab> prefab)
    {
        for (auto& prefab_inst : GameState::GetGameState().GetObjectsOfType<PrefabInstance>())
        {
            if (prefab_inst.prefab != prefab)
                continue;

            auto go = prefab_inst.GetGameObject();

            // store current value of overrides
            vector<reflect::dynamic> curr_ov_vals;
            for (auto& ov : prefab_inst.overrides)
            {
                const auto comp_handle = helpers::get_component(go, ov.component_name, ov.component_nth);
                if (!comp_handle)
                    curr_ov_vals.emplace_back();
                else
                {
                    auto dyn = resolve_property_path(*comp_handle, ov.property_path);
                    curr_ov_vals.emplace_back(dyn.type.create() = dyn);
                }
            }
            vector<reflect::dynamic> default_ov_vals;
            for (auto& ov : helpers::default_overrides)
            {
                const auto comp_handle = helpers::get_component(go, ov.component_name, ov.component_nth);
                if (!comp_handle)
                    default_ov_vals.emplace_back();
                else
                {
                    auto dyn = resolve_property_path(*comp_handle, ov.property_path);
                    default_ov_vals.emplace_back(dyn.type.create() = dyn);
                }
            }

            // replace prefab objects with instance objects
            int counters[ComponentCount]{ 0 };
			for (const auto& c : prefab_inst.prefab->data[prefab_inst.object_index].components)
			{
                auto index = GameState::GetGameState().GetTypeID(c.type) - 1;
                if (const auto handle = helpers::get_component(go, c.type.name(), counters[index]++)) // if component exists
                {
                    // memberwise copy
                    auto dyn = *handle;
                    for (size_t prop_index = 0; prop_index < dyn.type.count(); ++prop_index)
                        dyn.get_property(prop_index).value = c.get_property(prop_index).value;
                }
			}

            // replace override values
            for (size_t i = 0; i < curr_ov_vals.size(); ++i)
            {
                if (!curr_ov_vals[i].valid())
                    continue;
                auto& ov = prefab_inst.overrides[i];
                const auto comp_handle = helpers::get_component(go, ov.component_name, ov.component_nth);
                assign_property_path(*comp_handle, ov.property_path, curr_ov_vals[i]);
            }
            for (int i = 0; i < helpers::num_default_overrides; ++i)
            {
                if (!default_ov_vals[i].valid())
                    continue;
                auto& ov = helpers::default_overrides[i];
                const auto comp_handle = helpers::get_component(go, ov.component_name, ov.component_nth);
                assign_property_path(*comp_handle, ov.property_path, default_ov_vals[i]);
            }
        }
    }

    void PrefabUtility::PropagatePropertyToInstances(RscHandle<Prefab> prefab, int object_index,
                                                     string_view component_name, string_view property_path, int component_nth)
    {
        PropertyOverride prop_override{ string{component_name}, string{property_path}, component_nth };
        for (auto& prefab_inst : GameState::GetGameState().GetObjectsOfType<PrefabInstance>())
        {
            if (prefab_inst.prefab != prefab)
                continue;
            if (helpers::has_override(prefab_inst, component_name, property_path, component_nth))
                continue;
            if (helpers::is_default_override(prop_override, object_index == 0))
                continue;

            assign_property_path(*helpers::get_component(prefab_inst.GetGameObject(), component_name, component_nth), property_path,
                resolve_property_path(prefab->data[object_index].FindComponent(component_name, component_nth), property_path));
        }
    }

    void PrefabUtility::AddComponentToPrefab(RscHandle<Prefab> prefab, int object_index, reflect::dynamic component)
    {
        auto& data = prefab->data[object_index].components;
        const auto tid = GameState::GetGameState().GetTypeID(component.type);

        data.emplace_back();
        size_t i = data.size() - 1;
        for (; i > 0; --i)
        {
            if (GameState::GetGameState().GetTypeID(data[i - 1].type) <= tid)
                break;
            data[i - 1].swap(data[i]);
        }
        data[i].swap(component.copy());

        helpers::propagate_added_component(prefab, object_index, static_cast<int>(i));
        prefab->Dirty();
    }

    void PrefabUtility::RemoveComponentFromPrefab(RscHandle<Prefab> prefab, int object_index, int component_index)
    {
        auto& data = prefab->data[object_index];
        const auto nth = data.GetComponentNth(component_index);

        helpers::propagate_removed_component(prefab, object_index, data.components[component_index].type.name(), nth);
        data.components.erase(data.components.begin() + component_index);
        prefab->Dirty();
    }

    void PrefabUtility::RecordPrefabInstanceChange(Handle<GameObject> target, GenericHandle component, string_view property_path, reflect::dynamic val)
    {
        auto prefab_inst = target->GetComponent<PrefabInstance>();
        IDK_ASSERT(prefab_inst);

        const auto tname = (*component).type.name();
        const int component_nth = helpers::instance_component_to_prefab_data_component_nth(target, component);
        if (prefab_inst->prefab->data[prefab_inst->object_index].GetComponentIndex(tname, component_nth) == -1)
            return; // newly added, don't record

        int ov_i = helpers::find_override(*prefab_inst, tname, property_path, component_nth);
        if (ov_i == -1) // override not found
        {
            ov_i = static_cast<int>(prefab_inst->overrides.size());
            prefab_inst->overrides.push_back({ string(tname), string(property_path), component_nth });
        }

        if (!val.valid())
            val.swap(resolve_property_path(*component, property_path));
        prefab_inst->overrides[ov_i].value.swap(val.copy());
    }

    void PrefabUtility::RecordPrefabInstanceChangeForced(
        Handle<GameObject> target, string_view component_name, int component_nth, string_view property_path, reflect::dynamic val)
    {
        auto prefab_inst = target->GetComponent<PrefabInstance>();
        IDK_ASSERT(prefab_inst);

        component_nth = helpers::instance_component_nth_to_prefab_data_component_nth(target, component_name, component_nth);

        int ov_i = helpers::find_override(*prefab_inst, component_name, property_path, component_nth);
        if (ov_i == -1) // override not found
        {
            ov_i = static_cast<int>(prefab_inst->overrides.size());
            prefab_inst->overrides.push_back({ string(component_name), string(property_path), component_nth });
        }

        prefab_inst->overrides[ov_i].value.swap(val.copy());
    }

    bool PrefabUtility::RecordPrefabInstanceRemoveComponent(Handle<GameObject> target, string_view component_name, int component_nth)
    {
        auto prefab_inst = target->GetComponent<PrefabInstance>();
        IDK_ASSERT(prefab_inst);

        component_nth = helpers::instance_component_nth_to_prefab_data_component_nth(target, component_name, component_nth);

        // if -1, component is not part of prefab
        if (prefab_inst->prefab->data[prefab_inst->object_index].GetComponentIndex(component_name, component_nth) >= 0)
        {
            prefab_inst->removed_components.push_back({ component_name, component_nth });
            return true;
        }

        return false;
    }

    void PrefabUtility::RecordDefaultOverrides(Handle<GameObject> target)
    {
        helpers::add_default_overrides(*target->GetComponent<PrefabInstance>());
    }

    static void _revert_property_override(PrefabInstance& prefab_inst, const PropertyOverride& override)
    {
        if (helpers::is_default_override(override, prefab_inst.object_index == 0))
            return;

        const Prefab& prefab = *prefab_inst.prefab;
        const Handle<GameObject> target = prefab_inst.GetGameObject();

        const auto comp_handle = helpers::get_component(target, override.component_name, override.component_nth);
        if (!comp_handle)
            return;

        const auto nth = helpers::instance_component_nth_to_prefab_data_component_nth(target, override.component_name, override.component_nth);
        auto prop_prefab = resolve_property_path(
            prefab.data[prefab_inst.object_index].FindComponent(override.component_name, nth), override.property_path);
        assign_property_path(*comp_handle, override.property_path, prop_prefab);
    }

    void PrefabUtility::RevertPropertyOverride(Handle<GameObject> target, const PropertyOverride& override)
    {
        IDK_ASSERT(target->HasComponent<PrefabInstance>());
        auto& prefab_inst = *target->GetComponent<PrefabInstance>();
        _revert_property_override(prefab_inst, override);

        const auto nth = helpers::instance_component_nth_to_prefab_data_component_nth(target, override.component_name, override.component_nth);
        for (auto iter = prefab_inst.overrides.begin(); iter != prefab_inst.overrides.end(); ++iter)
        {
            if (iter->component_name == override.component_name &&
                iter->property_path == override.property_path &&
                iter->component_nth == nth)
            {
                prefab_inst.overrides.erase(iter);
                return;
            }
        }
    }

    void PrefabUtility::RevertRemovedComponent(Handle<GameObject> target, int component_index, GenericHandle force_handle)
    {
        auto prefab_inst = target->GetComponent<PrefabInstance>();
        IDK_ASSERT(prefab_inst);

        auto data = prefab_inst->prefab->data[prefab_inst->object_index];
        GenericHandle comp = force_handle == GenericHandle{} ?
            comp = target->AddComponent(data.components[component_index]) :
            comp = target->AddComponent(force_handle, data.components[component_index]);

        const auto cname = data.components[component_index].type.name();
        const auto nth = data.GetComponentNth(component_index);

        for (auto iter = prefab_inst->removed_components.begin(); iter != prefab_inst->removed_components.end(); ++iter)
        {
            if (iter->component_nth == nth && iter->component_name == cname)
            {
                prefab_inst->removed_components.erase(iter);
                break;
            }
        }

        target->SetComponentIndex(comp, nth);

        for (const auto& ov : prefab_inst->overrides)
        {
            if (ov.value.valid() && ov.component_nth == nth && ov.component_name == cname)
            {
                if (ov.value.valid())
                    assign_property_path(*comp, ov.property_path, ov.value);
            }
        }

    }

    void PrefabUtility::RevertPrefabInstance(Handle<GameObject> instance_root)
    {
        auto prefab_inst_handle = instance_root->GetComponent<PrefabInstance>();
        IDK_ASSERT(prefab_inst_handle && prefab_inst_handle->object_index == 0); // must be root
        auto& prefab_inst = *prefab_inst_handle;

        for (auto& override : prefab_inst.overrides)
            _revert_property_override(prefab_inst, override);
        prefab_inst.overrides.clear();

        vector<Handle<GameObject>> objs;
        auto tree = Core::GetSystem<SceneManager>().FetchSceneGraphFor(instance_root);
        tree.Visit([&objs](Handle<GameObject> child, int) { objs.push_back(child); });

        // find missing objs
        for (int i = 0; i < prefab_inst.prefab->data.size(); ++i)
        {
            auto iter = std::find_if(objs.begin(), objs.end(), [i](Handle<GameObject> obj)
            {
                if (const auto inst = obj->GetComponent<PrefabInstance>())
                {
                    if (inst->object_index == i)
                        return true;
                }
                return false;
            });
            if (iter != objs.end()) // object still exists
                continue;

            // spawn child
            auto child = Scene{ instance_root.scene }.CreateGameObject();
            auto child_inst = child->AddComponent<PrefabInstance>();
            child_inst->prefab = prefab_inst.prefab;
            child_inst->object_index = i;

            InstantiateSpecific(child, *child_inst);
            const auto parent_index = prefab_inst.prefab->data[i].parent_index;
            child->Transform()->SetParent(*std::find_if(objs.begin(), objs.end(), [parent_index](Handle<GameObject> obj)
            {
                if (const auto inst = obj->GetComponent<PrefabInstance>())
                {
                    if (inst->object_index == parent_index)
                        return true;
                }
                return false;
            }));
        }

        // diff components
        for (auto obj : objs)
        {
            auto obj_inst = obj->GetComponent<PrefabInstance>();
            if (!obj_inst)
                continue;

            for (auto& override : obj_inst->overrides)
            {
                _revert_property_override(*obj_inst, override);
            }
            obj_inst->overrides.clear();

            vector<int> removed, added;
            GetPrefabInstanceComponentDiff(obj, removed, added);
            auto comps = obj->GetComponents();

            // backwards so indices are not invalidated
            for (auto iter = added.rbegin(); iter != added.rend(); ++iter)
                obj->RemoveComponent(comps[*iter]);

            for (auto i : removed)
                RevertRemovedComponent(obj, i);

            helpers::add_default_overrides(*obj_inst);
        }

        helpers::add_default_overrides(prefab_inst);
    }

	void PrefabUtility::ApplyAddedComponent(Handle<GameObject> target, GenericHandle component)
	{
        IDK_ASSERT(target->GetComponent<PrefabInstance>());
        auto prefab_inst = target->GetComponent<PrefabInstance>();

        auto& data = prefab_inst->prefab->data[prefab_inst->object_index].components;
        const auto tid = component.type;

        data.emplace_back();
        size_t i = data.size() - 1;
        for (; i > 0; --i)
        {
            if (GameState::GetGameState().GetTypeID(data[i - 1].type) <= tid)
                break;
            data[i - 1].swap(data[i]);
        }
        data[i].swap((*component).copy());

		helpers::propagate_added_component(prefab_inst, static_cast<int>(i));

        prefab_inst->prefab->Dirty();
	}

    void PrefabUtility::ApplyRemovedComponent(Handle<GameObject> target, string_view component_name, int component_nth)
    {
        IDK_ASSERT(target->GetComponent<PrefabInstance>());
        auto prefab_inst = target->GetComponent<PrefabInstance>();

        auto& data = prefab_inst->prefab->data[prefab_inst->object_index].components;
        auto index = component_nth;
        for (auto iter = data.begin(); iter != data.end(); ++iter)
        {
            if (iter->type.name() == component_name)
            {
                if (index == 0)
                {
                    data.erase(iter);
                    break;
                }
                --index;
            }
        }

        helpers::propagate_removed_component(prefab_inst, component_name, component_nth);
        prefab_inst->prefab->Dirty();
    }

	static void _apply_property_override(PrefabInstance& target, const PropertyOverride& override)
	{
		if (helpers::is_default_override(override, target.object_index == 0))
			return;

        const Handle<GameObject> go = target.GetGameObject();
		const auto comp_handle = helpers::get_component(go, override.component_name, override.component_nth);
		if (!comp_handle)
			return;

        const auto nth = helpers::instance_component_nth_to_prefab_data_component_nth(go, override.component_name, override.component_nth);
        assign_property_path(
            target.prefab->data[target.object_index].FindComponent(override.component_name, nth),
            override.property_path,
            resolve_property_path(*comp_handle, override.property_path));
	}

	void PrefabUtility::ApplyPropertyOverride(Handle<GameObject> target, const PropertyOverride& override)
	{
        const auto prefab_inst_handle = target->GetComponent<PrefabInstance>();
        IDK_ASSERT(target->GetComponent<PrefabInstance>());
        auto& prefab_inst = *prefab_inst_handle;

        _apply_property_override(prefab_inst, override);

        const auto nth = helpers::instance_component_nth_to_prefab_data_component_nth(target, override.component_name, override.component_nth);
		for (auto iter = prefab_inst.overrides.begin(); iter != prefab_inst.overrides.end(); ++iter)
		{
			if (iter->component_name == override.component_name &&
				iter->property_path == override.property_path &&
				iter->component_nth == nth)
			{
				prefab_inst.overrides.erase(iter);
				break;
			}
		}

        const int c_index = prefab_inst.prefab->data[prefab_inst.object_index].GetComponentIndex(override.component_name, nth);
        helpers::propagate_property(prefab_inst_handle, c_index, override.property_path);

        prefab_inst.prefab->Dirty();
	}

	void PrefabUtility::ApplyPrefabInstance(Handle<GameObject> instance_root)
	{
        auto prefab_inst_handle = instance_root->GetComponent<PrefabInstance>();
        IDK_ASSERT(prefab_inst_handle && prefab_inst_handle->object_index == 0); // must be root
        const auto& prefab = prefab_inst_handle->prefab;

        vector<Handle<GameObject>> objs;
        auto tree = Core::GetSystem<SceneManager>().FetchSceneGraphFor(instance_root);
        tree.Visit([&objs](Handle<GameObject> child, int) { objs.push_back(child); });

        // find missing objs
        for (int i = 0; i < prefab->data.size(); ++i)
        {
            auto iter = std::find_if(objs.begin(), objs.end(), [i](Handle<GameObject> obj)
            {
                if (const auto inst = obj->GetComponent<PrefabInstance>())
                {
                    if (inst->object_index == i)
                        return true;
                }
                return false;
            });
            if (iter != objs.end()) // object still exists
                continue;

            // remove obj from prefab, and make sure links are still correct
            prefab->data.erase(prefab->data.begin() + i);
            for (auto obj : objs)
            {
                if (const auto inst = obj->GetComponent<PrefabInstance>())
                {
                    if (inst->object_index > i)
                        --inst->object_index;
                }
            }
        }

        // diff components
        for (auto obj : objs)
        {
            auto obj_prefab_inst = obj->GetComponent<PrefabInstance>();
            if (!obj_prefab_inst)
            {
                auto parent = obj->Parent();
                auto parent_inst = parent->GetComponent<PrefabInstance>();
                obj_prefab_inst = obj->AddComponent<PrefabInstance>();
                obj_prefab_inst->object_index = static_cast<int>(prefab->data.size());
                obj_prefab_inst->prefab = prefab;
                auto& obj_prefab_data = prefab->data.emplace_back();
                for (auto& c : obj->GetComponents())
                {
                    if (c.is_type<PrefabInstance>())
                        continue;
                    obj_prefab_data.components.emplace_back((*c).copy());
                }
                prefab->data.back().parent_index = parent_inst->object_index;
                helpers::add_default_overrides(*obj_prefab_inst);
                continue;
            }
            if (obj_prefab_inst->prefab != prefab)
                continue;

            auto& prefab_data = prefab->data[obj_prefab_inst->object_index];
            vector<int> removed, added;
            GetPrefabInstanceComponentDiff(obj, removed, added);

            // propagate all removes then remove from prefab data
            for (int i : removed)
            {
                helpers::propagate_removed_component(
                    obj_prefab_inst, prefab_data.components[i].type.name(), prefab_data.GetComponentNth(i));
            }
            for (int i = 0; i < removed.size(); ++i)
            {
                const int swap_begin = removed[removed.size() - i - 1];
                const int swap_end = static_cast<int>(prefab_data.components.size() - i);
                for (int j = swap_begin; j < swap_end - 1; ++j)
                    prefab_data.components[j].swap(prefab_data.components[j + 1]);
            }
            prefab_data.components.erase(prefab_data.components.end() - removed.size(), prefab_data.components.end());
            obj_prefab_inst->removed_components.clear();

            // add after remove, because add can invalidate to-be-removed ptrs
            for (const auto i : added)
                ApplyAddedComponent(obj, obj->GetComponents()[i]);

            for (const auto& override : obj_prefab_inst->overrides)
            {
                _apply_property_override(*obj_prefab_inst, override);
                int c_index = prefab->data[obj_prefab_inst->object_index].GetComponentIndex(override.component_name, override.component_nth);
                helpers::propagate_property(obj_prefab_inst, c_index, override.property_path);
            }
            obj_prefab_inst->overrides.clear();
            helpers::add_default_overrides(*obj_prefab_inst);
        }

        prefab->Dirty();
	}

}