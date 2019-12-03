//////////////////////////////////////////////////////////////////////////////////
//@file		CMD_SelectGameObject.h
//@author	Muhammad Izha B Rahim
//@param	Email : izha95\@hotmail.com
//@date		03 DEC 2019
//@brief	Selects gameobject, does not call on first execute
//////////////////////////////////////////////////////////////////////////////////


#include <editor/commands/ICommand.h>
#include <core/GameObject.h>

namespace idk {

	class CMD_SelectGameObject : public ICommand { //This command is saved when mouse is released, after moving.
	public:
		CMD_SelectGameObject(Handle<GameObject> gameObject);

		virtual bool execute() override;
		virtual bool undo() override;

	private:
		//vector<Handle<GameObject>> previous_selected_state{};
		//vector<Handle<GameObject>> current_selected_state{};
		bool		first_execute			{ false };
	};

}