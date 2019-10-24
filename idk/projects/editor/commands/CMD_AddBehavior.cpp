#include "pch.h"
#include "CMD_AddBehavior.h"
#include <script/MonoBehavior.h>

namespace idk
{
	CMD_AddBehavior::CMD_AddBehavior(Handle<GameObject> gameObject, string behavior)
		: CMD_AddComponent{ gameObject, string{"MonoBehavior"} }, behavior_name{ behavior }
	{
	}

	CMD_AddBehavior::CMD_AddBehavior(Handle<GameObject> gameObject, const mono::Behavior& behavior)
		: CMD_AddComponent{ gameObject, behavior.GetHandle() }, behavior_name {behavior.TypeName()}
	{
		saved_string = serialize_text(behavior.GetObject());
	}
	bool CMD_AddBehavior::execute()
	{
		if (!game_object_handle)
			return false;

		CMD_AddComponent::execute();
		IDK_ASSERT(new_component_handle);

		auto mb_handle = handle_cast<mono::Behavior>(new_component_handle);
		mb_handle->EmplaceBehavior(behavior_name);
		if (saved_string.size())
			parse_text(saved_string, mb_handle->GetObject());
	}
	bool CMD_AddBehavior::undo()
	{
		return CMD_AddComponent::undo();
	}
}
