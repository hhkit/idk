//////////////////////////////////////////////////////////////////////////////////
//@file		CMD_SelectGameObject.h
//@author	Muhammad Izha B Rahim
//@param	Email : izha95\@hotmail.com
//@date		03 DEC 2019
//@brief	Selects gameobject, does not call on first execute
//////////////////////////////////////////////////////////////////////////////////


#include <editor/commands/ICommand.h>
#include <editor/ObjectSelection.h>

namespace idk 
{
	class CMD_SelectObject : public ICommand 
	{ //This command is saved when mouse is released, after moving.
	public:
		CMD_SelectObject(ObjectSelection new_selection);

		virtual bool execute() override;
		virtual bool undo() override;

	private:
		ObjectSelection old_selection;
		ObjectSelection new_selection;
	};

}