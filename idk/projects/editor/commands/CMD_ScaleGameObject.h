//////////////////////////////////////////////////////////////////////////////////
//@file		CMD_ScaleGameObject.h
//@author	Muhammad Izha B Rahim
//@param	Email : izha95\@hotmail.com
//@date		10 SEPT 2019
//@brief	Adds components to gameobjects
//////////////////////////////////////////////////////////////////////////////////


#include <editor/commands/ICommand.h>
#include <core/GameObject.h>

namespace idk {

	class CMD_ScaleGameObject : public ICommand { //This command is saved when mouse is released, after moving.
	public:
		CMD_ScaleGameObject(Handle<GameObject> gameObject, const vec3& sca);

		virtual bool execute() override;
		virtual bool undo() override;

	private:
		const vec3			scale				{};
	};

}