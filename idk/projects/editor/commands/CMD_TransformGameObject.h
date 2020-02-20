//////////////////////////////////////////////////////////////////////////////////
//@file		CMD_MoveGameObject.h
//@author	Muhammad Izha B Rahim
//@param	Email : izha95\@hotmail.com
//@date		10 SEPT 2019
//@brief	Keeps gameobjects transform values. Does not call on first execute.
//			That means you have to modify the values before saving them.
//////////////////////////////////////////////////////////////////////////////////


#include <editor/commands/ICommand.h>
#include <core/GameObject.h>
#include <prefab/PrefabInstance.h>

namespace idk {

	class CMD_TransformGameObject : public ICommand { //This command is saved when mouse is released, after moving.
	public:
		CMD_TransformGameObject(Handle<GameObject> gameObject, const mat4& matrix_old, const mat4& matrix_new);

		virtual bool execute() override;
		virtual bool undo() override;

	private:
		mat4		original_values			{};
		mat4		new_values				{};
		vector<PropertyOverride> overrides_old;
	};

}