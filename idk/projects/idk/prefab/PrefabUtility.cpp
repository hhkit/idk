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
        static void add_component(Handle<GameObject> go, const reflect::dynamic& prefab_comp)
        {
            if (prefab_comp.is<Transform>())
            {
                auto& t_prefab = prefab_comp.get<Transform>();
                auto& t_ori = *go->GetComponent<Transform>();
                t_ori.position = t_prefab.position;
                t_ori.rotation = t_prefab.rotation;
                t_ori.scale = t_prefab.scale;
                t_ori.parent = t_prefab.parent;
            }
            else if (prefab_comp.is<Name>())
                go->GetComponent<Name>()->name = prefab_comp.get<Name>().name;
            else
                go->AddComponent(prefab_comp);
        }

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
                        curr.~dynamic();
                        new (&curr) reflect::dynamic(cont[parse_text(string(token), key_type)]);
                    }
                    else
                    {
                        curr.~dynamic();
                        new (&curr) reflect::dynamic(cont[parse_text<size_t>(string(token))]);
                    }
                }
                else
                {
                    curr.~dynamic();
                    new (&curr) reflect::dynamic(obj.get_property(token).value);
                }

                offset = end;
                ++offset;
            }

            return curr;
        }

        const static PropertyOverride default_overrides[3]{
            { -1, "Transform", "position" }, { -1, "Transform", "rotation" },
            { -1, "Name", "name" }
        };
        constexpr static auto num_default_overrides = 3;

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

		static bool has_override(PrefabInstance& prefab_inst, int object_index, string_view component_name, string_view property_path)
		{
			for (auto& ov : prefab_inst.overrides)
			{
				if (ov.object_index != object_index || ov.component_name == component_name)
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


		static void propagate_property(Handle<GameObject> origin, RscHandle<Prefab> prefab,
                                       int object_index, int component_index, string_view property_path)
		{
			for (auto& prefab_inst : GameState::GetGameState().GetObjectsOfType<PrefabInstance>())
			{
				if (prefab_inst.prefab != prefab || prefab_inst.GetGameObject() == origin)
					continue;

				const auto component_name = prefab->data[object_index].components[component_index].type.name();
				if (!has_override(prefab_inst, object_index, component_name, property_path))
				{
					resolve_property_path(*prefab_inst.objects[object_index]->GetComponent(component_name), property_path) =
						resolve_property_path(prefab->data[object_index].components[component_index], property_path);
				}
			}
		}

		static void propagate_added_component(Handle<GameObject> origin, RscHandle<Prefab> prefab, int object_index, int component_index)
		{
			for (auto& prefab_inst : GameState::GetGameState().GetObjectsOfType<PrefabInstance>())
			{
				if (prefab_inst.prefab != prefab || prefab_inst.GetGameObject() == origin)
					continue;

				prefab_inst.objects[object_index]->AddComponent(prefab->data[object_index].components[component_index]);
			}
		}

        static void propagate_removed_component(Handle<GameObject> origin, RscHandle<Prefab> prefab,
                                                int object_index, string_view component_name, int component_add_index)
        {
            for (auto& prefab_inst : GameState::GetGameState().GetObjectsOfType<PrefabInstance>())
            {
                if (prefab_inst.prefab != prefab || prefab_inst.GetGameObject() == origin)
                    continue;

                for (auto c : prefab_inst.objects[object_index]->GetComponents())
                {
                    if ((*c).type.name() == component_name)
                    {
                        if (component_add_index == 0)
                        {
                            prefab_inst.objects[object_index]->RemoveComponent(c);
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

        auto handle = scene.CreateGameObject();
        auto iter = prefab->data.begin();
        for (const auto& d : iter->components)
            helpers::add_component(handle, d);

        vector<Handle<GameObject>> game_objects{ handle };
        for (++iter; iter != prefab->data.end(); ++iter)
        {
            const auto child_handle = scene.CreateGameObject();
            game_objects.push_back(child_handle);

            for (const auto& d : iter->components)
                helpers::add_component(child_handle, d);
            child_handle->Transform()->parent = game_objects[iter->parent_index];
        }

        auto& prefab_inst = *handle->AddComponent<PrefabInstance>();
        prefab_inst.prefab = prefab;
        prefab_inst.objects = game_objects;

        return handle;
    }

    static void _create(Handle<GameObject> go, RscHandle<Prefab> handle)
    {
        auto& prefab = *handle;
        prefab.data.clear();
        auto& root = prefab.data.emplace_back();
        for (auto& c : go->GetComponents())
        {
            root.components.emplace_back((*c).copy());
        }

        // build tree
        Scene& scene = *Core::GetSystem<SceneManager>().GetSceneByBuildIndex(go.scene);

        vector<small_string<GenericHandle::index_t>> nodes;
        vector<GenericHandle::gen_t> gens;
        for (const auto& o : scene)
        {
            const auto index = s_cast<size_t>(o.GetHandle().index);
            if (index >= gens.size())
            {
                nodes.resize(index + 1);
                gens.resize(index + 1);
            }
            gens[index] = o.GetHandle().gen;
            if (o.Parent())
                nodes[o.Parent().index] += index;
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
                for (auto& c : child->GetComponents())
                    child_prefab_data.components.emplace_back((*c).copy());
                child_prefab_data.parent_index = static_cast<int>(game_objects.find(curr_par));
            }
        }
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

            // store current value of overrides
            vector<reflect::dynamic> curr_ov_vals;
            for (auto& ov : prefab_inst.overrides)
            {
                const auto comp_handle = prefab_inst.objects[ov.object_index]->GetComponent(ov.component_name);
                if (!comp_handle)
                    curr_ov_vals.emplace_back();
                else
                {
                    auto dyn = helpers::resolve_property_path(*comp_handle, ov.property_path);
                    curr_ov_vals.emplace_back(dyn.type.create() = dyn);
                }
            }
            vector<reflect::dynamic> default_ov_vals;
            for (int i = 0; i < prefab_inst.objects.size(); ++i)
            {
                for (auto& ov : helpers::default_overrides)
                {
                    const auto comp_handle = prefab_inst.objects[i]->GetComponent(ov.component_name);
                    if (!comp_handle)
                        default_ov_vals.emplace_back();
                    else
                    {
                        auto dyn = helpers::resolve_property_path(*comp_handle, ov.property_path);
                        default_ov_vals.emplace_back(dyn.type.create() = dyn);
                    }
                }
            }

            // replace prefab objects with instance objects
            for (size_t i = 0; i < prefab_inst.objects.size(); ++i)
            {
                auto& obj = prefab_inst.objects[i];
				for (const auto& c : prefab_inst.prefab->data[i].components)
				{
                    if (auto handle = obj->GetComponent(c.type)) // if component exists
                    {
                        auto dyn = *handle;
                        for (size_t prop_index = 0; prop_index < dyn.type.count(); ++prop_index)
                            dyn.get_property(prop_index).value = c.get_property(prop_index).value;
                    }
				}
            }

            // replace override values
            for (size_t i = 0; i < curr_ov_vals.size(); ++i)
            {
                if (!curr_ov_vals[i].valid())
                    continue;
                auto& ov = prefab_inst.overrides[i];
                auto obj = prefab_inst.objects[ov.object_index];
                if (!obj)
                    continue;
                const auto comp_handle = obj->GetComponent(ov.component_name);
                helpers::resolve_property_path(*comp_handle, ov.property_path) = curr_ov_vals[i];
            }
            for (int i = 0; i < prefab_inst.objects.size(); ++i)
            {
                for (int j = 0; j < helpers::num_default_overrides; ++j)
                {
                    const int ov_index = helpers::num_default_overrides * i + j;
                    if (!default_ov_vals[ov_index].valid())
                        continue;
                    auto& ov = helpers::default_overrides[j];
                    auto obj = prefab_inst.objects[i];
                    if (!obj)
                        continue;
                    const auto comp_handle = obj->GetComponent(ov.component_name);
                    helpers::resolve_property_path(*comp_handle, ov.property_path) = default_ov_vals[ov_index];
                }
            }
        }
    }

    void PrefabUtility::RecordPrefabInstanceChange(Handle<GameObject> target, GenericHandle component, string_view property_path)
    {
        const auto instance_root = GetPrefabInstanceRoot(target);
        assert(instance_root);
        auto& prefab_inst = *instance_root->GetComponent<PrefabInstance>();

        auto iter = std::find(prefab_inst.objects.begin(), prefab_inst.objects.end(), target);
        if (iter == prefab_inst.objects.end())
            return;

        PropertyOverride override;
        override.object_index = static_cast<int>(iter - prefab_inst.objects.begin());
        override.component_name = (*component).type.name();
        override.property_path = property_path;

        if (helpers::is_default_override(override))
            return;

        // check if there is already such an override
        // or if there is already an override of the property's parent
        // ie. scale when the property_path passed in is scale/x
        for (auto& ov : prefab_inst.overrides)
        {
            if (ov.object_index == override.object_index && ov.component_name == override.component_name)
            {
                if (ov.property_path.size() == override.property_path.size())
                {
                    if (ov.property_path == override.property_path)
                        return;
                }
                else if (ov.property_path.size() > override.property_path.size()) // eg. scale/x vs scale
                {
                    if (ov.property_path.find(override.property_path) == 0 && ov.property_path[override.property_path.size()] == '/')
                    {
                        std::swap(ov.property_path, override.property_path); // make ov the shorter one
                        return;
                    }
                }
                else // eg. scale vs scale/x
                {
                    if (ov.property_path.find(override.property_path) == 0 && ov.property_path[override.property_path.size()] == '/')
                        return;
                }
            }
        }

        prefab_inst.overrides.push_back(override);
    }

    static void _revert_property_override(PrefabInstance& prefab_inst, const PropertyOverride& override)
    {
        if (helpers::is_default_override(override))
            return;

        const Prefab& prefab = *prefab_inst.prefab;
        const Handle<GameObject> target = prefab_inst.objects[override.object_index];

        const auto comp_handle = target->GetComponent(override.component_name);
        if (!comp_handle)
            return;

        auto prop = helpers::resolve_property_path(*comp_handle, override.property_path);
        if (!prop.valid())
            return;

        auto prop_prefab = helpers::resolve_property_path(
            prefab.data[override.object_index].FindComponent(override.component_name), override.property_path);
        if (!prop_prefab.valid())
            return;

        prop = prop_prefab;
    }

    void PrefabUtility::RevertPropertyOverride(Handle<GameObject> instance_root, const PropertyOverride& override)
    {
        assert(instance_root->HasComponent<PrefabInstance>());
        auto& prefab_inst = *instance_root->GetComponent<PrefabInstance>();
        _revert_property_override(prefab_inst, override);

        for (auto iter = prefab_inst.overrides.begin(); iter != prefab_inst.overrides.end(); ++iter)
        {
            if (iter->object_index == override.object_index &&
                iter->component_name == override.component_name &&
                iter->property_path == override.property_path)
            {
                prefab_inst.overrides.erase(iter);
                return;
            }
        }
    }

    void PrefabUtility::RevertPrefabInstance(Handle<GameObject> instance_root)
    {
        assert(instance_root->HasComponent<PrefabInstance>());
        auto& prefab_inst = *instance_root->GetComponent<PrefabInstance>();

        for (auto& override : prefab_inst.overrides)
            _revert_property_override(prefab_inst, override);
        prefab_inst.overrides.clear();
    }

	void PrefabUtility::ApplyAddedComponent(Handle<GameObject> target, GenericHandle component)
	{
		const auto instance_root = GetPrefabInstanceRoot(target);
		assert(instance_root);
		auto& prefab_inst = *instance_root->GetComponent<PrefabInstance>();

		auto index = std::find(prefab_inst.objects.begin(), prefab_inst.objects.end(), target) - prefab_inst.objects.begin();
		prefab_inst.prefab->data[index].components.push_back((*component).copy());

		helpers::propagate_added_component(instance_root, prefab_inst.prefab, static_cast<int>(index),
                                           static_cast<int>(prefab_inst.prefab->data[index].components.size() - 1));

        prefab_inst.prefab->Dirty();
	}

    void PrefabUtility::ApplyRemovedComponent(Handle<GameObject> target, string_view component_name, int component_add_index)
    {
        const auto instance_root = GetPrefabInstanceRoot(target);
        assert(instance_root);
        auto& prefab_inst = *instance_root->GetComponent<PrefabInstance>();

        auto index = std::find(prefab_inst.objects.begin(), prefab_inst.objects.end(), target) - prefab_inst.objects.begin();
        auto& data = prefab_inst.prefab->data[index].components;
        for (auto iter = data.begin(); iter != data.end(); ++iter)
        {
            if (iter->type.name() == component_name)
            {
                if (component_add_index == 0)
                {
                    data.erase(iter);
                    break;
                }
                --component_add_index;
            }
        }

        helpers::propagate_removed_component(instance_root, prefab_inst.prefab, static_cast<int>(index), component_name, component_add_index);
        prefab_inst.prefab->Dirty();
    }

	static void _apply_property_override(PrefabInstance& prefab_inst, const PropertyOverride& override)
	{
		if (helpers::is_default_override(override))
			return;

		Prefab& prefab = *prefab_inst.prefab;

		auto prop_prefab = helpers::resolve_property_path(
			prefab.data[override.object_index].FindComponent(override.component_name), override.property_path);
		if (!prop_prefab.valid())
			return;

		const Handle<GameObject> target = prefab_inst.objects[override.object_index];

		const auto comp_handle = target->GetComponent(override.component_name);
		if (!comp_handle)
			return;

		auto prop = helpers::resolve_property_path(*comp_handle, override.property_path);
		if (!prop.valid())
			return;

		prop_prefab = prop;
	}

	void PrefabUtility::ApplyPropertyOverride(Handle<GameObject> instance_root, const PropertyOverride& override)
	{
		assert(instance_root->HasComponent<PrefabInstance>());
		auto& prefab_inst = *instance_root->GetComponent<PrefabInstance>();
		_apply_property_override(prefab_inst, override);

		for (auto iter = prefab_inst.overrides.begin(); iter != prefab_inst.overrides.end(); ++iter)
		{
			if (iter->object_index == override.object_index &&
				iter->component_name == override.component_name &&
				iter->property_path == override.property_path)
			{
				prefab_inst.overrides.erase(iter);
				break;
			}
		}

        helpers::propagate_property(instance_root,
                                    prefab_inst.prefab,
                                    override.object_index,
                                    prefab_inst.prefab->data[override.object_index].GetComponentIndex(override.component_name),
                                    override.property_path);
        prefab_inst.prefab->Dirty();
	}

	void PrefabUtility::ApplyPrefabInstance(Handle<GameObject> instance_root)
	{
        assert(instance_root->HasComponent<PrefabInstance>());
        auto& prefab_inst = *instance_root->GetComponent<PrefabInstance>();
        const auto& prefab = prefab_inst.prefab;

        // diff components
        int i = 0;
        for (auto obj : prefab_inst.objects)
        {
            vector<reflect::dynamic> obj_components;
            vector<GenericHandle> obj_component_handles;
            vector<reflect::dynamic*> prefab_component_ptrs;
            for (auto& d : prefab->data[i].components)
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

            i++;
        }

        for (auto& override : prefab_inst.overrides)
        {
            _apply_property_override(prefab_inst, override);

            helpers::propagate_property(instance_root,
                                        prefab,
                                        override.object_index,
                                        prefab->data[override.object_index].GetComponentIndex(override.component_name),
                                        override.property_path);
        }
        prefab_inst.overrides.clear();
        prefab_inst.prefab->Dirty();
	}

}