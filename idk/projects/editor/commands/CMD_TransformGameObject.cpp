//////////////////////////////////////////////////////////////////////////////////
//@file		CMD_MoveGameObject.cpp
//@author	Muhammad Izha B Rahim
//@param	Email : izha95\@hotmail.com
//@date		10 SEPT 2019
//@brief	Moves gameobjects
//////////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include <editor/commands/CMD_TransformGameObject.h>
#include <common/Transform.h>
#include <prefab/PrefabUtility.h>

namespace idk {

	CMD_TransformGameObject::CMD_TransformGameObject(Handle<GameObject> gameObject, const mat4& matrix_old, const mat4& matrix_new)
		: original_values{ matrix_old }, new_values{ matrix_new }
	{
		game_object_handle = gameObject;
	}

	bool CMD_TransformGameObject::execute()
	{
		if (!game_object_handle)
			return false;

		auto trans = game_object_handle->Transform();

		auto old_t = *trans;
		trans->GlobalMatrix(new_values);
		if (const auto prefab_inst = game_object_handle->GetComponent<PrefabInstance>())
		{
			overrides_old = prefab_inst->overrides;

			if (old_t.position != trans->position)
				PrefabUtility::RecordPrefabInstanceChange(game_object_handle, trans, "position", trans->position);
			if (old_t.rotation != trans->rotation)
				PrefabUtility::RecordPrefabInstanceChange(game_object_handle, trans, "rotation", trans->rotation);
			if (old_t.scale != trans->scale)
				PrefabUtility::RecordPrefabInstanceChange(game_object_handle, trans, "scale", trans->scale);
		}
		return true;
	}

	bool CMD_TransformGameObject::undo()
	{
		if (!game_object_handle)
			return false;
		game_object_handle->Transform()->GlobalMatrix(original_values);
		if (const auto prefab_inst = game_object_handle->GetComponent<PrefabInstance>())
			prefab_inst->overrides = overrides_old;
		return true;
	}


}
