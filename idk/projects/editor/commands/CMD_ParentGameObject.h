#pragma once
#include <editor/commands/ICommand.h>
#include <core/GameObject.h>

namespace idk {

	class CMD_ParentGameObject : public ICommand { //serialize/deserialize use text.h
	public:
		CMD_ParentGameObject(Handle<GameObject> gameObject, Handle<GameObject> new_parent);

		virtual bool execute() override;
		virtual bool undo() override;
	protected:
		Handle<GameObject> go;
		Handle<GameObject> old_parent;
		Handle<GameObject> new_parent;
	};

}