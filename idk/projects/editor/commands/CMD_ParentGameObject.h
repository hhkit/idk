#pragma once
#include <editor/commands/ICommand.h>
#include <core/GameObject.h>
#include <prefab/PrefabInstance.h>

namespace idk {

	class CMD_ParentGameObject : public ICommand 
	{
	public:
		CMD_ParentGameObject(Handle<GameObject> gameObject, Handle<GameObject> new_parent);

		virtual bool execute() override;
		virtual bool undo() override;

	protected:
		Handle<GameObject> go;
		Handle<GameObject> old_parent;
		Handle<GameObject> new_parent;

		hash_table<Handle<GameObject>, PrefabInstance> old_prefab_instance_comps;
	};

}