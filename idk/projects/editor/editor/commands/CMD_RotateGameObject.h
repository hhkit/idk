//////////////////////////////////////////////////////////////////////////////////
//@file		CMD_RotateGameObject.h
//@author	Muhammad Izha B Rahim
//@param	Email : izha95\@hotmail.com
//@date		30 AUG 2019
//@brief	Adds components to gameobjects
//////////////////////////////////////////////////////////////////////////////////


#include <editor/commands/ICommand.h>
#include <core/GameObject.h>

namespace idk {


	class CMD_RotateGameObject : public ICommand { //This command is saved when mouse is released, after moving.
	public:
		CMD_RotateGameObject(Handle<GameObject> gameObject, const quat& rot);

		virtual bool execute() override;
		virtual bool undo() override;

	private:
		Handle<GameObject>	game_object_handle	{};
		const quat			rotation			{};
	};



}