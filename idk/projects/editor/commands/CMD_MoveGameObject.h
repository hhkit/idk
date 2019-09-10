//////////////////////////////////////////////////////////////////////////////////
//@file		CMD_MoveGameObject.h
//@author	Muhammad Izha B Rahim
//@param	Email : izha95\@hotmail.com
//@date		10 SEPT 2019
//@brief	Moves gameobjects
//////////////////////////////////////////////////////////////////////////////////


#include <editor/commands/ICommand.h>
#include <core/GameObject.h>

namespace idk {

	class CMD_MoveGameObject : public ICommand { //This command is saved when mouse is released, after moving.
	public:
		CMD_MoveGameObject(Handle<GameObject> gameObject, const vec3& pos);

		virtual bool execute() override;
		virtual bool undo() override;

	private:
		const vec3			position			{};
	};

}