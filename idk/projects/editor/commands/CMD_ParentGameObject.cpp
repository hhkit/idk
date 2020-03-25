#include "pch.h"
#include "CMD_ParentGameObject.h"
#include <common/Transform.h>
#include <prefab/PrefabUtility.h>
#include <scene/SceneManager.h>
#include <scene/SceneGraph.inl>

namespace idk
{
	CMD_ParentGameObject::CMD_ParentGameObject(Handle<GameObject> gameObject, Handle<GameObject> new_parent)
		: go{gameObject}, old_parent{gameObject->Parent()}, new_parent{new_parent}
	{
	}

	bool CMD_ParentGameObject::execute()
	{
		if (!go)
			return false;

		old_prefab_instance_comps.clear();

		const auto prefab_root = PrefabUtility::GetPrefabInstanceRoot(go);
		if (prefab_root != go && prefab_root) // if reparenting an instance child, break the prefab instance
		{
			Core::GetSystem<SceneManager>().FetchSceneGraphFor(prefab_root).Visit([&](Handle<GameObject> h, int)
			{
				if (const auto prefab_inst = h->GetComponent<PrefabInstance>())
					old_prefab_instance_comps.emplace(h, *prefab_inst);
			});
			PrefabUtility::BreakPrefabInstance(prefab_root);
		}

		go->Transform()->SetParent(new_parent, true);

		return true;
	}

	bool CMD_ParentGameObject::undo()
	{
		if (!go)
			return false;

		go->Transform()->SetParent(old_parent, true);

		if (old_prefab_instance_comps.size())
		{
			for (const auto& [h, prefab_inst] : old_prefab_instance_comps)
			{
				h->AddComponent(prefab_inst.GetHandle(), prefab_inst);
			}
		}

		return true;
	}
}