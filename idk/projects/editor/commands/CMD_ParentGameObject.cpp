#include "pch.h"
#include "CMD_ParentGameObject.h"
#include <common/Transform.h>
namespace idk
{
	CMD_ParentGameObject::CMD_ParentGameObject(Handle<GameObject> gameObject, Handle<GameObject> new_parent)
		: go{gameObject}, old_parent{gameObject->Parent()}, new_parent{new_parent}
	{
	}
	bool CMD_ParentGameObject::execute()
	{
		go->Transform()->SetParent(new_parent, true);
		return true;
	}
	bool CMD_ParentGameObject::undo()
	{
		go->Transform()->SetParent(old_parent, true);
		return true;
	}
}