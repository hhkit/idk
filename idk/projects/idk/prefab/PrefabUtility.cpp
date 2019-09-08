#include "stdafx.h"
#include "PrefabUtility.h"

#include <ds/small_string.h>
#include <serialize/serialize.h>
#include <file/FileSystem.h>
#include <core/GameObject.h>
#include <common/Transform.h>
#include <common/Name.h>
#include <prefab/Prefab.h>
#include <prefab/PrefabInstance.h>

namespace idk
{

    namespace helpers
    {
        static void set_component(Handle<GameObject> go, const reflect::dynamic& prefab_comp)
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

        static reflect::dynamic resolve_property_path(const reflect::dynamic& obj, const string& path)
        {
            size_t offset = 0;
            reflect::dynamic curr;

            while (offset < path.size())
            {
                auto end = path.find('/', offset);
                if (end == string::npos)
                    end = path.size();
                string_view token(path.data() + offset, end - offset);

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
    }


    Handle<GameObject> PrefabUtility::Instantiate(RscHandle<Prefab> prefab, Scene& scene)
    {
        if (!prefab)
            return Handle<GameObject>();

        auto handle = scene.CreateGameObject();
        auto iter = prefab->data.begin();
        for (const auto& d : iter->components)
            helpers::set_component(handle, d);

        vector<Handle<GameObject>> game_objects{ handle };
        for (++iter; iter != prefab->data.end(); ++iter)
        {
            auto child_handle = scene.CreateGameObject();
            game_objects.push_back(child_handle);

            for (const auto& d : iter->components)
                helpers::set_component(child_handle, d);
            child_handle->Transform()->parent = game_objects[iter->parent_index];
        }

        auto prefab_inst = handle->AddComponent<PrefabInstance>();
        prefab_inst->prefab = prefab;
        prefab_inst->objects = game_objects;

        return handle;
    }

    void PrefabUtility::Save(Handle<GameObject> go, FileHandle save_path)
    {
        Prefab prefab;
        auto& root = prefab.data.emplace_back();
        for (auto& c : go->GetComponents())
        {
            root.components.emplace_back(*c);
        }

        // build tree
        Scene scene{ go.scene };
        vector<small_string<GenericHandle::index_t>> nodes;
        vector<GenericHandle::gen_t> gens;
        for (auto& o : scene)
        {
            auto index = o.GetHandle().index;
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
                Handle<GameObject> child{ child_index, gens[child_index], go.scene };
                PrefabData& child_prefab_data = prefab.data.emplace_back();
                for (auto& c : child->GetComponents())
                {
					child_prefab_data.components.emplace_back(*c);
                }
                child_prefab_data.parent_index = static_cast<int>(game_objects.find(curr_par));
            }
        }

        save_path.Open(FS_PERMISSIONS::WRITE) << serialize_text(prefab.data);
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
        for (uint8_t scene_index = 0; scene_index < MaxScene; ++scene_index)
        {
            Scene scene{ scene_index };
            if (!GameState::GetGameState().ValidateScene(scene))
                continue;

            for (auto& go : scene)
            {
                auto prefab_inst = go.GetComponent<PrefabInstance>();
                if (!prefab_inst || prefab_inst->prefab != prefab)
                    continue;

                // store current value of overrides
                vector<reflect::dynamic> curr_ov_vals;
                for (auto& ov : prefab_inst->overrides)
                {
                    auto comp_handle = prefab_inst->objects[ov.object_index]->GetComponent(ov.component_name);
                    if (!comp_handle)
                        curr_ov_vals.emplace_back();
                    else
                    {
                        auto dyn = helpers::resolve_property_path(*comp_handle, ov.property_path);
                        curr_ov_vals.emplace_back(dyn.type.create() = dyn);
                    }
                }
                vector<reflect::dynamic> default_ov_vals;
                for (int i = 0; i < prefab_inst->objects.size(); ++i)
                {
                    for (auto& ov : helpers::default_overrides)
                    {
                        auto comp_handle = prefab_inst->objects[i]->GetComponent(ov.component_name);
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
                for (size_t i = 0; i < prefab_inst->objects.size(); ++i)
                {
                    auto& obj = prefab_inst->objects[i];
                    for (const auto& c : prefab_inst->prefab->data[i].components)
                        helpers::set_component(obj, c);
                }

                // replace override values
                for (size_t i = 0; i < curr_ov_vals.size(); ++i)
                {
                    if (!curr_ov_vals[i].valid())
                        continue;
                    auto& ov = prefab_inst->overrides[i];
                    auto obj = prefab_inst->objects[ov.object_index];
                    if (!obj)
                        continue;
                    auto comp_handle = obj->GetComponent(ov.component_name);
                    helpers::resolve_property_path(*comp_handle, ov.property_path) = curr_ov_vals[i];
                }
                for (int i = 0; i < prefab_inst->objects.size(); ++i)
                {
                    for (int j = 0; j < helpers::num_default_overrides; ++j)
                    {
                        int ov_index = helpers::num_default_overrides * i + j;
                        if (!default_ov_vals[ov_index].valid())
                            continue;
                        auto& ov = helpers::default_overrides[j];
                        auto obj = prefab_inst->objects[i];
                        if (!obj)
                            continue;
                        auto comp_handle = obj->GetComponent(ov.component_name);
                        helpers::resolve_property_path(*comp_handle, ov.property_path) = default_ov_vals[ov_index];
                    }
                }
            }

            *(reinterpret_cast<uint8_t*>(&scene) + sizeof(Resource<Scene>)) = 7;//hack
        }
    }

    void PrefabUtility::RecordPrefabInstanceChange(Handle<GameObject> target, GenericHandle component, string_view property_path)
    {
        auto instance_root = GetPrefabInstanceRoot(target);
        assert(instance_root);
        auto prefab_inst = instance_root->GetComponent<PrefabInstance>();

        auto iter = std::find(prefab_inst->objects.begin(), prefab_inst->objects.end(), target);
        if (iter == prefab_inst->objects.end())
            return;

        PropertyOverride override;
        override.object_index = static_cast<int>(iter - prefab_inst->objects.begin());
        override.component_name = (*component).type.name();
        override.property_path = property_path;

        if (helpers::is_default_override(override))
            return;

        // check if there is already such an override
        // or if there is already an override of the property's parent
        // ie. scale when the property_path passed in is scale/x
        for (auto& ov : prefab_inst->overrides)
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

        prefab_inst->overrides.push_back(override);
    }

    static void _revert_property_override(PrefabInstance& prefab_inst, const PropertyOverride& override)
    {
        if (helpers::is_default_override(override))
            return;

        const Prefab& prefab = *prefab_inst.prefab;
        Handle<GameObject> target = prefab_inst.objects[override.object_index];

        auto comp_handle = target->GetComponent(override.component_name);
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
        auto prefab_inst = *instance_root->GetComponent<PrefabInstance>();

        for (auto& override : prefab_inst.overrides)
            _revert_property_override(prefab_inst, override);
        prefab_inst.overrides.clear();
    }

}