#include "stdafx.h"
#include "PrefabUtility.h"

#include <ds/small_string.h>
#include <serialize/text.h>
#include <file/FileSystem.h>
#include <core/GameObject.h>
#include <common/Transform.h>
#include <common/Name.h>
#include <scene/SceneManager.h>
#include <script/MonoBehavior.h>
#include <prefab/Prefab.h>
#include <prefab/PrefabInstance.h>
#include <script/ManagedObj.h>
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

                if (curr.valid())
                {
					if (curr.type.is<mono::ManagedObject>())
						return curr;
                    if (curr.type.is_container())
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
                    else if (curr.type.is_template<std::variant>())
                        curr.swap(curr.get_variant_value().get_property(token).value);
                    else
                        curr.swap(obj.get_property(token).value);
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

		static bool has_override(PrefabInstance& prefab_inst, string_view component_name, string_view property_path, int component_nth)
		{
			for (auto& ov : prefab_inst.overrides)
			{
				if (ov.component_name != component_name)
					continue;
                if (ov.component_nth != component_nth)
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
					resolve_property_path(*helpers::get_component(prefab_inst.GetGameObject(), component_name, component_nth), property_path) =
						resolve_property_path(comp, property_path);
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
            auto prefab = origin->prefab;
            for (auto& prefab_inst : GameState::GetGameState().GetObjectsOfType<PrefabInstance>())
            {
                if (prefab_inst.prefab != prefab ||
                    prefab_inst.GetGameObject() == origin->GetGameObject() ||
                    prefab_inst.object_index != origin->object_index)
                    continue;

                for (auto c : prefab_inst.GetGameObject()->GetComponents())
                {
                    if ((*c).type.name() == component_name)
                    {
                        if (component_nth == 0)
                        {
                            prefab_inst.GetGameObject()->RemoveComponent(c);
                            break;
                        }
                        --component_nth;
                    }
                }
            }
        }
        static void propagate_removed_component(RscHandle<Prefab> prefab, int object_index, string_view component_name, int component_nth)
        {
            for (auto& prefab_inst : GameState::GetGameState().GetObjectsOfType<PrefabInstance>())
            {
                if (prefab_inst.prefab != prefab ||
                    prefab_inst.object_index != object_index)
                    continue;

                for (auto c : prefab_inst.GetGameObject()->GetComponents())
                {
                    if ((*c).type.name() == component_name)
                    {
                        if (component_nth == 0)
                        {
                            prefab_inst.GetGameObject()->RemoveComponent(c);
                            break;
                        }
                        --component_nth;
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
            helpers::add_component(handle, d);
        auto prefab_inst = handle->AddComponent<PrefabInstance>();
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

	RscHandle<Prefab> PrefabUtility::Create(Handle<GameObject> go, Guid guid)
	{
		auto handle = Core::GetResourceManager().LoaderCreateResource<Prefab>(guid);
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
        const auto& prefab = *obj_prefab_inst->prefab;

        auto& prefab_data = prefab.data[obj_prefab_inst->object_index];
        vector<GenericHandle> obj_component_handles;
        vector<const reflect::dynamic*> prefab_component_ptrs;
        obj_component_handles.reserve(target->GetComponents().size());
        prefab_component_ptrs.reserve(prefab_data.components.size());

        for (auto& d : prefab_data.components)
            prefab_component_ptrs.push_back(&d);
        for (auto c : target->GetComponents())
            obj_component_handles.push_back(c);

        for (size_t i = 0; i < prefab_component_ptrs.size(); ++i)
        {
            const auto tid = GameState::GetGameState().GetTypeID(prefab_component_ptrs[i]->type);
            for (size_t j = 0; j < obj_component_handles.size(); ++j)
            {
                if (tid == obj_component_handles[j].type)
                {
                    prefab_component_ptrs[i] = nullptr;
                    obj_component_handles[j] = GenericHandle();
                    break;
                }
            }
        }

        // ptrs left in prefab comp ptrs == removed components
        // ptrs left in obj comp ptrs == added components
        for (int i = 0; i < prefab_component_ptrs.size(); ++i)
        {
            if (prefab_component_ptrs[i])
                out_removed.push_back(i);
        }
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
                    auto dyn = helpers::resolve_property_path(*comp_handle, ov.property_path);
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
                    auto dyn = helpers::resolve_property_path(*comp_handle, ov.property_path);
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
                helpers::resolve_property_path(*comp_handle, ov.property_path) = curr_ov_vals[i];
            }
            for (int i = 0; i < helpers::num_default_overrides; ++i)
            {
                if (!default_ov_vals[i].valid())
                    continue;
                auto& ov = helpers::default_overrides[i];
                const auto comp_handle = helpers::get_component(go, ov.component_name, ov.component_nth);
                helpers::resolve_property_path(*comp_handle, ov.property_path) = default_ov_vals[i];
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
            if (helpers::is_default_override(prop_override))
                continue;

            helpers::resolve_property_path(*helpers::get_component(prefab_inst.GetGameObject(), component_name, component_nth), property_path) =
                helpers::resolve_property_path(prefab->data[object_index].FindComponent(component_name, component_nth), property_path);
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
        auto& data = prefab->data[object_index].components;
        auto type = data[component_index].type;
        int add_index = 0;
        for (size_t i = 0; i < component_index; ++i)
        {
            if (data[i].type == type)
                ++add_index;
        }
        helpers::propagate_removed_component(prefab, object_index, type.name(), add_index);
        data.erase(data.begin() + component_index);
        prefab->Dirty();
    }

    void PrefabUtility::RecordPrefabInstanceChange(Handle<GameObject> target, GenericHandle component, string_view property_path)
    {
        auto prefab_inst = target->GetComponent<PrefabInstance>();
        assert(prefab_inst);

        const span components = target->GetComponents();
        int component_nth = -1;
        for (const auto& c : components)
        {
            if (c.type == component.type)
                ++component_nth;
            if (c == component)
                break;
        }
        PropertyOverride override{ string((*component).type.name()), string(property_path), component_nth };

        if (helpers::is_default_override(override))
            return;
        if (helpers::has_override(*prefab_inst, override.component_name, property_path, component_nth))
            return;

        prefab_inst->overrides.push_back(override);
    }

    static void _revert_property_override(PrefabInstance& prefab_inst, const PropertyOverride& override)
    {
        if (helpers::is_default_override(override))
            return;

        const Prefab& prefab = *prefab_inst.prefab;
        const Handle<GameObject> target = prefab_inst.GetGameObject();

        const auto comp_handle = helpers::get_component(target, override.component_name, override.component_nth);
        if (!comp_handle)
            return;

        auto prop = helpers::resolve_property_path(*comp_handle, override.property_path);
        if (!prop.valid())
            return;

        auto prop_prefab = helpers::resolve_property_path(
            prefab.data[prefab_inst.object_index].FindComponent(override.component_name, override.component_nth), override.property_path);
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
                iter->property_path == override.property_path &&
                iter->component_nth == override.component_nth)
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
        assert(target->GetComponent<PrefabInstance>());
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
		if (helpers::is_default_override(override))
			return;

		Prefab& prefab = *target.prefab;

		auto prop_prefab = helpers::resolve_property_path(
			prefab.data[target.object_index].FindComponent(override.component_name, override.component_nth), override.property_path);
		if (!prop_prefab.valid())
			return;

        const Handle<GameObject> go = target.GetGameObject();

		const auto comp_handle = helpers::get_component(go, override.component_name, override.component_nth);
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
				iter->property_path == override.property_path &&
				iter->component_nth == override.component_nth)
			{
				prefab_inst.overrides.erase(iter);
				break;
			}
		}

        const int c_index = prefab_inst.prefab->data[prefab_inst.object_index].GetComponentIndex(override.component_name, override.component_nth);
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

            auto& prefab_data = prefab->data[obj_prefab_inst->object_index];
            vector<int> removed, added;
            GetPrefabInstanceComponentDiff(obj, removed, added);

            // ptrs left in obj comp ptrs == added components
            // ptrs left in prefab comp ptrs == removed components

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
        }

        prefab->Dirty();
	}

}