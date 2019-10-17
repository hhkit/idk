#include "stdafx.h"
#include "PrefabUtility.h"

#include <ds/small_string.h>
#include <serialize/serialize.h>
#include <file/FileSystem.h>
#include <core/GameObject.h>
#include <common/Transform.h>
#include <common/Name.h>
#include <scene/SceneManager.h>
#include <prefab/Prefab.h>
#include <prefab/PrefabInstance.h>

namespace idk
{

    namespace helpers
    {
        static reflect::dynamic resolve_property_path(const reflect::dynamic& obj, string_view path)
        {
            size_t offset = 0;
            reflect::dynamic curr;

            while (offset < path.size())
            {
                auto end = path.find('/', offset);
                if (end == string::npos)
                    end = path.size();
                const string_view token(path.data() + offset, end - offset);

                if (curr.valid() && curr.type.is_container())
                {
                    auto cont = curr.to_container();
                    if (cont.value_type.is_template<std::pair>())
                    {
                        auto key_type = cont.value_type.create().unpack()[0].type;
                        curr.swap(cont[*parse_text(string(token), key_type)]);
                    }
                    else
                        curr.swap(cont[*parse_text<size_t>(string(token))]);
                }
                else
                    curr.swap(obj.get_property(token).value);

                offset = end;
                ++offset;
            }

            return curr;
        }

        const static PropertyOverride default_overrides[]{
            { "Transform", "position" }, { "Transform", "rotation" }, { "Transform", "parent" },
            { "Name", "name" }
        };
        constexpr static auto num_default_overrides = std::size(default_overrides);

        static bool is_default_override(const PropertyOverride& ov)
        {
            if (ov.component_name == "Transform")
            {
                if (ov.property_path.find("position") == 0 || ov.property_path.find("rotation") == 0)
                    return true;
            }
            else if (ov.component_name == "Name")
                return true;

            return false;
        }

		static bool has_override(PrefabInstance& prefab_inst, string_view component_name, string_view property_path)
		{
			for (auto& ov : prefab_inst.overrides)
			{
				if (ov.component_name == component_name)
					continue;

				if (ov.property_path.size() == property_path.size())
				{
					if (ov.property_path == property_path)
						return true;
				}
				else if (ov.property_path.size() > property_path.size()) // eg. scale/x vs scale
				{
					if (ov.property_path.find(property_path) == 0 && ov.property_path[property_path.size()] == '/')
					{
						ov.property_path = property_path; // make ov the shorter one
						return true;
					}
				}
				else // eg. scale vs scale/x
				{
					if (ov.property_path.find(property_path) == 0 && ov.property_path[property_path.size()] == '/')
						return true;
				}
			}
			
			return false;
		}


        static void propagate_property(Handle<PrefabInstance> origin, int component_index, string_view property_path)
		{
            auto prefab = origin->prefab;
			for (auto& prefab_inst : GameState::GetGameState().GetObjectsOfType<PrefabInstance>())
			{
				if (prefab_inst.prefab != prefab || prefab_inst.GetGameObject() == origin->GetGameObject())
					continue;

                auto& comp = prefab->data[prefab_inst.object_index].components[component_index];
				const auto component_name = comp.type.name();
				if (!has_override(prefab_inst, component_name, property_path))
				{
					resolve_property_path(*prefab_inst.GetGameObject()->GetComponent(component_name), property_path) =
						resolve_property_path(comp, property_path);
				}
			}
		}

		static void propagate_added_component(Handle<PrefabInstance> origin, int component_index)
		{
            auto prefab = origin->prefab;
			for (auto& prefab_inst : GameState::GetGameState().GetObjectsOfType<PrefabInstance>())
			{
				if (prefab_inst.prefab != prefab || prefab_inst.GetGameObject() == origin->GetGameObject())
					continue;

				prefab_inst.GetGameObject()->AddComponent(
                    prefab->data[prefab_inst.object_index].components[component_index]);
			}
		}
        static void propagate_added_component(RscHandle<Prefab> prefab, int component_index)
        {
            for (auto& prefab_inst : GameState::GetGameState().GetObjectsOfType<PrefabInstance>())
            {
                if (prefab_inst.prefab != prefab)
                    continue;

                prefab_inst.GetGameObject()->AddComponent(
                    prefab->data[prefab_inst.object_index].components[component_index]);
            }
        }

        static void propagate_removed_component(Handle<PrefabInstance> origin, string_view component_name, int component_add_index)
        {
            auto prefab = origin->prefab;
            for (auto& prefab_inst : GameState::GetGameState().GetObjectsOfType<PrefabInstance>())
            {
                if (prefab_inst.prefab != prefab || prefab_inst.GetGameObject() == origin->GetGameObject())
                    continue;

                for (auto c : prefab_inst.GetGameObject()->GetComponents())
                {
                    if ((*c).type.name() == component_name)
                    {
                        if (component_add_index == 0)
                        {
                            prefab_inst.GetGameObject()->RemoveComponent(c);
                            break;
                        }
                        --component_add_index;
                    }
                }
            }
        }
        static void propagate_removed_component(RscHandle<Prefab> prefab, string_view component_name, int component_add_index)
        {
            for (auto& prefab_inst : GameState::GetGameState().GetObjectsOfType<PrefabInstance>())
            {
                if (prefab_inst.prefab != prefab)
                    continue;

                for (auto c : prefab_inst.GetGameObject()->GetComponents())
                {
                    if ((*c).type.name() == component_name)
                    {
                        if (component_add_index == 0)
                        {
                            prefab_inst.GetGameObject()->RemoveComponent(c);
                            break;
                        }
                        --component_add_index;
                    }
                }
            }
        }
    }



    Handle<GameObject> PrefabUtility::Instantiate(RscHandle<Prefab> prefab, Scene& scene)
    {
        if (!prefab)
            return Handle<GameObject>();

        const auto handle = scene.CreateGameObject();
        const auto& prefab_data = prefab->data;
        auto i = 0;
        for (const auto& d : prefab_data[i].components)
            handle->AddComponent(d);
        auto prefab_inst = handle->AddComponent<PrefabInstance>();
        prefab_inst->prefab = prefab;
        prefab_inst->object_index = i;

        vector<Handle<GameObject>> game_objects{ handle };
        while (++i < prefab_data.size())
        {
            const auto child_handle = scene.CreateGameObject();
            game_objects.push_back(child_handle);

            for (const auto& d : prefab_data[i].components)
                child_handle->AddComponent(d);
            child_handle->Transform()->parent = game_objects[prefab_data[i].parent_index];

            prefab_inst = child_handle->AddComponent<PrefabInstance>();
            prefab_inst->prefab = prefab;
            prefab_inst->object_index = i;
        }

        // root name should be based on prefab asset name/path, not namecomponent
        if (prefab->Name().size())
            handle->Name(prefab->Name());
        else if (auto path = Core::GetResourceManager().GetPath(prefab))
            handle->Name(PathHandle(*path).GetStem());

        return handle;
    }

    static void _create(Handle<GameObject> go, RscHandle<Prefab> prefab_handle, bool connect_inst = false)
    {
        if (connect_inst)
        {
            auto prefab_inst = go->AddComponent<PrefabInstance>();
            prefab_inst->prefab = prefab_handle;
            prefab_inst->object_index = 0;
        }

        auto& prefab = *prefab_handle;
        prefab.data.clear();
        auto& root = prefab.data.emplace_back();
        for (auto& c : go->GetComponents())
        {
            if (c.is_type<PrefabInstance>())
                continue;
            root.components.emplace_back((*c).copy());
        }

        // build tree
        Scene& scene = *Core::GetSystem<SceneManager>().GetSceneByBuildIndex(go.scene);

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
            if (o.Parent())
                nodes[o.Parent().index] += static_cast<GenericHandle::index_t>(index);
        }

        // tree walk
        small_string<GenericHandle::index_t> stack(1, go.index);
        small_string<GenericHandle::index_t> game_objects;
        while (stack.size())
        {
            auto curr_par = stack.back();
            stack.pop_back();
            stack += nodes[curr_par];
            game_objects += curr_par;

            for (auto child_index : nodes[curr_par])
            {
                const Handle<GameObject> child{ child_index, gens[child_index], go.scene };
                PrefabData& child_prefab_data = prefab.data.emplace_back();
                if (connect_inst)
                {
                    auto prefab_inst = child->AddComponent<PrefabInstance>();
                    prefab_inst->prefab = prefab_handle;
                    prefab_inst->object_index = static_cast<int>(prefab.data.size() - 1);
                }
                for (auto& c : child->GetComponents())
                {
                    child_prefab_data.components.emplace_back((*c).copy());
                }
                child_prefab_data.parent_index = static_cast<int>(game_objects.find(curr_par));
            }
        }

        prefab_handle->Name(go->Name());
    }

    RscHandle<Prefab> PrefabUtility::Create(Handle<GameObject> go)
    {
        auto handle = Core::GetResourceManager().Create<Prefab>();
        _create(go, handle);
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

        _create(go, handle);
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

        _create(go, handle, true);
        return handle;
    }

    Handle<GameObject> PrefabUtility::GetPrefabInstanceRoot(Handle<GameObject> go)
    {
        while (go)
        {
            if (go->HasComponent<PrefabInstance>())
                return go;
            go = go->Parent();
        }
        return go;
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
                const auto comp_handle = go->GetComponent(ov.component_name);
                if (!comp_handle)
                    curr_ov_vals.emplace_back();
                else
                {
                    auto dyn = helpers::resolve_property_path(*comp_handle, ov.property_path);
                    curr_ov_vals.emplace_back(dyn.type.create() = dyn);
                }
            }
            vector<reflect::dynamic> default_ov_vals;
            for (auto& ov : helpers::default_overrides)
            {
                const auto comp_handle = go->GetComponent(ov.component_name);
                if (!comp_handle)
                    default_ov_vals.emplace_back();
                else
                {
                    auto dyn = helpers::resolve_property_path(*comp_handle, ov.property_path);
                    default_ov_vals.emplace_back(dyn.type.create() = dyn);
                }
            }

            // replace prefab objects with instance objects
			for (const auto& c : prefab_inst.prefab->data[prefab_inst.object_index].components)
			{
                if (const auto handle = go->GetComponent(c.type)) // if component exists
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
                const auto comp_handle = go->GetComponent(ov.component_name);
                helpers::resolve_property_path(*comp_handle, ov.property_path) = curr_ov_vals[i];
            }
            for (int i = 0; i < helpers::num_default_overrides; ++i)
            {
                if (!default_ov_vals[i].valid())
                    continue;
                auto& ov = helpers::default_overrides[i];
                const auto comp_handle = go->GetComponent(ov.component_name);
                helpers::resolve_property_path(*comp_handle, ov.property_path) = default_ov_vals[i];
            }
        }
    }

    void PrefabUtility::PropagatePropertyToInstances(RscHandle<Prefab> prefab, int object_index, string_view component_name, string_view property_path)
    {
        PropertyOverride prop_override{ string{component_name}, string{property_path} };
        for (auto& prefab_inst : GameState::GetGameState().GetObjectsOfType<PrefabInstance>())
        {
            if (prefab_inst.prefab != prefab)
                continue;
            if (helpers::has_override(prefab_inst, component_name, property_path))
                continue;
            if (helpers::is_default_override(prop_override))
                continue;

            helpers::resolve_property_path(*prefab_inst.GetGameObject()->GetComponent(component_name), property_path) =
                helpers::resolve_property_path(prefab->data[object_index].FindComponent(component_name), property_path);
        }
    }

    void PrefabUtility::AddComponentToPrefab(RscHandle<Prefab> prefab, int object_index, reflect::dynamic component)
    {
        auto& data = prefab->data[object_index].components;
        data.push_back(component);
        helpers::propagate_added_component(prefab, static_cast<int>(data.size() - 1));
    }

    void PrefabUtility::RemoveComponentFromPrefab(RscHandle<Prefab> prefab, int object_index, int component_index)
    {
        auto& data = prefab->data[object_index].components;
        auto type = data[component_index].type;
        int add_index = 0;
        for (size_t i = 0; i < component_index; ++i)
        {
            if (data[i].type == type)
                ++add_index;
        }
        helpers::propagate_removed_component(prefab, type.name(), add_index);
        data.erase(data.begin() + component_index);
    }

    void PrefabUtility::RecordPrefabInstanceChange(Handle<GameObject> target, GenericHandle component, string_view property_path)
    {
        auto prefab_inst = target->GetComponent<PrefabInstance>();
        assert(prefab_inst);

        PropertyOverride override;
        override.component_name = (*component).type.name();
        override.property_path = property_path;

        if (helpers::is_default_override(override))
            return;
        if (helpers::has_override(*prefab_inst, override.component_name, property_path))
            return;

        prefab_inst->overrides.push_back(override);
    }

    static void _revert_property_override(PrefabInstance& prefab_inst, const PropertyOverride& override)
    {
        if (helpers::is_default_override(override))
            return;

        const Prefab& prefab = *prefab_inst.prefab;
        const Handle<GameObject> target = prefab_inst.GetGameObject();

        const auto comp_handle = target->GetComponent(override.component_name);
        if (!comp_handle)
            return;

        auto prop = helpers::resolve_property_path(*comp_handle, override.property_path);
        if (!prop.valid())
            return;

        auto prop_prefab = helpers::resolve_property_path(
            prefab.data[prefab_inst.object_index].FindComponent(override.component_name), override.property_path);
        if (!prop_prefab.valid())
            return;

        prop = prop_prefab;
    }

    void PrefabUtility::RevertPropertyOverride(Handle<GameObject> target, const PropertyOverride& override)
    {
        assert(target->HasComponent<PrefabInstance>());
        auto& prefab_inst = *target->GetComponent<PrefabInstance>();
        _revert_property_override(prefab_inst, override);

        for (auto iter = prefab_inst.overrides.begin(); iter != prefab_inst.overrides.end(); ++iter)
        {
            if (iter->component_name == override.component_name &&
                iter->property_path == override.property_path)
            {
                prefab_inst.overrides.erase(iter);
                return;
            }
        }
    }

    void PrefabUtility::RevertPrefabInstance(Handle<GameObject> instance_root)
    {
        auto prefab_inst_handle = instance_root->GetComponent<PrefabInstance>();
        assert(prefab_inst_handle && prefab_inst_handle->object_index == 0); // must be root
        auto& prefab_inst = *prefab_inst_handle;

        for (auto& override : prefab_inst.overrides)
            _revert_property_override(prefab_inst, override);
        prefab_inst.overrides.clear();
    }

	void PrefabUtility::ApplyAddedComponent(Handle<GameObject> target, GenericHandle component)
	{
        assert(target->GetComponent<PrefabInstance>());
        auto prefab_inst = target->GetComponent<PrefabInstance>();

        auto& data = prefab_inst->prefab->data[prefab_inst->object_index].components;
        data.push_back((*component).copy());

		helpers::propagate_added_component(prefab_inst, static_cast<int>(data.size() - 1));

        prefab_inst->prefab->Dirty();
	}

    void PrefabUtility::ApplyRemovedComponent(Handle<GameObject> target, string_view component_name, int component_add_index)
    {
        assert(target->GetComponent<PrefabInstance>());
        auto prefab_inst = target->GetComponent<PrefabInstance>();

        auto& data = prefab_inst->prefab->data[prefab_inst->object_index].components;
        auto index = component_add_index;
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

        helpers::propagate_removed_component(prefab_inst, component_name, component_add_index);
        prefab_inst->prefab->Dirty();
    }

	static void _apply_property_override(PrefabInstance& target, const PropertyOverride& override)
	{
		if (helpers::is_default_override(override))
			return;

		Prefab& prefab = *target.prefab;

		auto prop_prefab = helpers::resolve_property_path(
			prefab.data[target.object_index].FindComponent(override.component_name), override.property_path);
		if (!prop_prefab.valid())
			return;

        const Handle<GameObject> go = target.GetGameObject();

		const auto comp_handle = go->GetComponent(override.component_name);
		if (!comp_handle)
			return;

		auto prop = helpers::resolve_property_path(*comp_handle, override.property_path);
		if (!prop.valid())
			return;

		prop_prefab = prop;
	}

	void PrefabUtility::ApplyPropertyOverride(Handle<GameObject> target, const PropertyOverride& override)
	{
        const auto prefab_inst_handle = target->GetComponent<PrefabInstance>();
        assert(target->GetComponent<PrefabInstance>());
        auto& prefab_inst = *prefab_inst_handle;
		_apply_property_override(prefab_inst, override);

		for (auto iter = prefab_inst.overrides.begin(); iter != prefab_inst.overrides.end(); ++iter)
		{
			if (iter->component_name == override.component_name &&
				iter->property_path == override.property_path)
			{
				prefab_inst.overrides.erase(iter);
				break;
			}
		}

        const int c_index = prefab_inst.prefab->data[prefab_inst.object_index].GetComponentIndex(override.component_name);
        helpers::propagate_property(prefab_inst_handle, c_index, override.property_path);

        prefab_inst.prefab->Dirty();
	}

	void PrefabUtility::ApplyPrefabInstance(Handle<GameObject> instance_root)
	{
        auto prefab_inst_handle = instance_root->GetComponent<PrefabInstance>();
        assert(prefab_inst_handle && prefab_inst_handle->object_index == 0); // must be root
        const auto& prefab = prefab_inst_handle->prefab;

        vector<Handle<GameObject>> objs;
        auto* tree = Core::GetSystem<SceneManager>().FetchSceneGraphFor(instance_root);
        tree->visit([&objs](Handle<GameObject> child, int) { objs.push_back(child); });

        // diff components
        for (auto obj : objs)
        {
            auto obj_prefab_inst = obj->GetComponent<PrefabInstance>();
            if (!obj_prefab_inst)
            {
                // todo: apply added obj
                continue;
            }
            if (obj_prefab_inst->prefab != prefab)
                continue;

            vector<reflect::dynamic> obj_components;
            vector<GenericHandle> obj_component_handles;
            vector<reflect::dynamic*> prefab_component_ptrs;
            obj_components.reserve(obj->GetComponents().size());
            obj_component_handles.reserve(obj->GetComponents().size());
            prefab_component_ptrs.reserve(prefab->data[obj_prefab_inst->object_index].components.size());

            for (auto& d : prefab->data[obj_prefab_inst->object_index].components)
                prefab_component_ptrs.push_back(&d);
            for (auto c : obj->GetComponents())
                obj_component_handles.push_back(c);
            for (auto c : obj_component_handles)
                obj_components.push_back(*c);

            for (size_t j = 0; j < prefab_component_ptrs.size(); ++j)
            {
                if (prefab_component_ptrs[j]->type == obj_components[j].type)
                {
                    prefab_component_ptrs[j] = nullptr;
                    obj_component_handles[j] = GenericHandle();
                }
            }

            // ptrs left in obj comp ptrs == added components
            // ptrs left in prefab comp ptrs == removed components

            for (auto c : obj_component_handles)
            {
                if (c && !c.is_type<PrefabInstance>())
                    ApplyAddedComponent(obj, c);
            }
            for (const auto* d : prefab_component_ptrs)
            {
                if (d)
                    ApplyRemovedComponent(obj, d->type.name(), 0);
            }

            for (auto & override : obj_prefab_inst->overrides)
            {
                _apply_property_override(*obj_prefab_inst, override);
                int c_index = prefab->data[obj_prefab_inst->object_index].GetComponentIndex(override.component_name);
                helpers::propagate_property(obj_prefab_inst, c_index, override.property_path);
            }
            obj_prefab_inst->overrides.clear();
        }

        prefab->Dirty();
	}

}