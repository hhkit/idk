//////////////////////////////////////////////////////////////////////////////////
//@file		CMD_CreateGameObject.h
//@author	Muhammad Izha B Rahim
//@param	Email : izha95\@hotmail.com
//@date		9 Sept 2019
//@brief	Adds components to gameobjects
//////////////////////////////////////////////////////////////////////////////////


#include <editor/commands/ICommand.h>
#include <core/GameObject.h>

namespace idk {

	class CMD_CreateGameObject : public ICommand { //serialize/deserialize use serialize.h
	public:
		CMD_CreateGameObject();

		virtual bool execute() override;

		virtual bool undo() override;

	private:
		Handle<GameObject>	new_game_object_handle	{};
	};

}