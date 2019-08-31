#include "stdafx.h"
#include "Prefab.h"
#include <core/GameObject.h>
#include <common/Parent.h>
#include <file/FileSystem.h>
#include <serialize/serialize.h>
#include <ds/small_string.h>

namespace idk
{
    // static

    void Prefab::Save(Handle<GameObject> go, FileHandle save_path)
    {
        Prefab prefab;
        for (auto& c : go->GetComponents())
        {
            prefab._data.components.emplace_back(*c);
        }

        // build tree
        Scene scene{ go.scene };
        vector<small_string<decltype(Handle<GameObject>::id)>> nodes{ /* scene.size() */ };
        for (auto& o : scene)
        {
            if (o.ParentObject())
                nodes[o.ParentObject().id] += o.GetHandle().id;
        }

        // tree walk
        small_string<decltype(Handle<GameObject>::id)> stack(1, go.id);
        vector<PrefabData*> prefab_data_stack{ &prefab._data };
        while (stack.size())
        {
            auto curr_par = stack.back();
            stack.pop_back();
            stack += nodes[curr_par];

            auto& curr_prefab_data = *prefab_data_stack.back();
            prefab_data_stack.pop_back();

            for (auto child_id : nodes[curr_par])
            {
                Handle<GameObject> child{ child_id };
                PrefabData& child_prefab_data = curr_prefab_data.children.emplace_back();
                for (auto& c : child->GetComponents())
                    child_prefab_data.components.emplace_back(*c);
                prefab_data_stack.push_back(&child_prefab_data);
            }
        }

        save_path.Open(FS_PERMISSIONS::WRITE) << serialize_text(prefab._data);
    }



    // members

    static Handle<GameObject> _instantiate(Scene& scene, const PrefabData& data)
    {
        auto handle = scene.CreateGameObject();
        for (const auto& d : data.components)
            handle->AddComponent(d);
        for (const auto& child : data.children)
            _instantiate(scene, child)->AddComponent<Parent>()->parent = handle;
        return handle;
    }
    Handle<GameObject> Prefab::Instantiate(Scene& scene) const
    {
        return _instantiate(scene, _data);
    }

}