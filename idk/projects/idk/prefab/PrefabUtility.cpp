#include "stdafx.h"
#include "PrefabUtility.h"

#include <ds/small_string.h>
#include <serialize/serialize.h>
#include <file/FileSystem.h>
#include <core/GameObject.h>
#include <common/Parent.h>
#include <prefab/Prefab.h>
#include <prefab/PrefabInstance.h>

namespace idk
{

    Handle<GameObject> PrefabUtility::Instantiate(RscHandle<Prefab> prefab, Scene& scene)
    {
        if (!prefab)
            return Handle<GameObject>();

        auto handle = scene.CreateGameObject();
        auto iter = prefab->data.begin();
        for (const auto& d : iter->components)
            handle->AddComponent(d);

        vector<Handle<GameObject>> game_objects{ handle };
        for (++iter; iter != prefab->data.end(); ++iter)
        {
            auto child_handle = scene.CreateGameObject();
            game_objects.push_back(child_handle);

            for (const auto& d : iter->components)
                child_handle->AddComponent(d);
            child_handle->AddComponent<Parent>()->parent = game_objects[iter->parent_index];
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
        vector<small_string<GenericHandle::index_t>> nodes{ /* scene.size() */ };
        vector<GenericHandle::gen_t> gens;
        for (auto& o : scene)
        {
            
            if (o.ParentObject())
                nodes[o.ParentObject().index] += o.GetHandle().index;
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
                Handle<GameObject> child{ child_index };
                PrefabData& child_prefab_data = prefab.data.emplace_back();
                for (auto& c : child->GetComponents())
                    child_prefab_data.components.emplace_back(*c);
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
            go = go->ParentObject();
        }
        return go;
    }

    static reflect::dynamic _resolve_property_path(const reflect::dynamic& obj, const string& path)
    {
        size_t offset = 0;
        reflect::dynamic curr;

        while (true)
        {
            auto end = path.find('/', offset);
            string_view token(path.data() + offset, end - offset);

            if (curr.type.is_container())
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
            if (offset == string::npos)
                break;
            else
                ++offset;
        }

        return curr;
    }

    static void _revert_property_override(PrefabInstance& prefab_inst, const PropertyOverride& override)
    {
        const Prefab& prefab = *prefab_inst.prefab;
        Handle<GameObject> target = prefab_inst.objects[override.object_index];
        auto comp = *target->GetComponent(override.component_name);
        auto prop = _resolve_property_path(comp, override.property_path);

        auto comp_type = reflect::get_type(override.component_name);
        for (auto& original_comp : prefab.data[override.object_index].components)
        {
            if (original_comp.type == comp_type)
            {
                prop = _resolve_property_path(original_comp, override.property_path);
                break;
            }
        }

        for (auto iter = prefab_inst.overrides.begin(); iter != prefab_inst.overrides.end(); ++iter)
        {
            if (&*iter == &override)
            {
                prefab_inst.overrides.erase(iter);
                return;
            }
        }
    }

    void PrefabUtility::RevertPropertyOverride(Handle<GameObject> instance_root, const PropertyOverride& override)
    {
        assert(instance_root->HasComponent<PrefabInstance>());
        _revert_property_override(*instance_root->GetComponent<PrefabInstance>(), override);
    }

    void PrefabUtility::RevertPrefabInstance(Handle<GameObject> instance_root)
    {
        assert(instance_root->HasComponent<PrefabInstance>());
        auto prefab_inst = *instance_root->GetComponent<PrefabInstance>();
        
        for (auto& override : prefab_inst.overrides)
        {
            _revert_property_override(prefab_inst, override);
        }
    }

}