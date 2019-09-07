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

    static void _set_component(Handle<GameObject> go, const reflect::dynamic& prefab_comp)
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

    Handle<GameObject> PrefabUtility::Instantiate(RscHandle<Prefab> prefab, Scene& scene)
    {
        if (!prefab)
            return Handle<GameObject>();

        auto handle = scene.CreateGameObject();
        auto iter = prefab->data.begin();
        for (const auto& d : iter->components)
            _set_component(handle, d);

        vector<Handle<GameObject>> game_objects{ handle };
        for (++iter; iter != prefab->data.end(); ++iter)
        {
            auto child_handle = scene.CreateGameObject();
            game_objects.push_back(child_handle);

            for (const auto& d : iter->components)
                _set_component(child_handle, d);
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

        prefab_inst->overrides.push_back(override);
    }

    static reflect::dynamic _resolve_property_path(const reflect::dynamic& obj, const string& path)
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

    static void _revert_property_override(PrefabInstance& prefab_inst, const PropertyOverride & override)
    {
        const Prefab& prefab = *prefab_inst.prefab;
        Handle<GameObject> target = prefab_inst.objects[override.object_index];

        auto comp_handle = target->GetComponent(override.component_name);
        if (!comp_handle)
            return;
        auto comp = *comp_handle;

        auto prop = _resolve_property_path(comp, override.property_path);
        if (!prop.valid())
            return;

        auto prop_prefab = _resolve_property_path(
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