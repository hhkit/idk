//////////////////////////////////////////////////////////////////////////////////
//@file		CMD_CollateCommands.cpp
//@author	Muhammad Izha B Rahim
//@param	Email : izha95\@hotmail.com
//@date		18 NOV 2019
//@brief	Repeats commands again when undo or redo is called.
/*			Place this after calling the command with the number of times it is called.
			EG:

			for (loop done 5 times) {
				COMMAND();
			}
			CallCommandAgain(5);

			This command directy overrides the CommandController
*/
//////////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include <editor/commands/CMD_CollateCommands.h>
#include <IDE.h>

namespace idk {


	CMD_CollateCommands::CMD_CollateCommands(int counts)
		: count{ counts }
	{
	}


	bool CMD_CollateCommands::execute()
	{
		if (repeated_commands.empty()) // first call
		{
			if (count <= 1)
				return false;

			CommandController& controller = Core::GetSystem<IDE>()._command_controller;
			for (auto i = 0; i < count; ++i)
			{
				repeated_commands.push_back(std::move(controller.undoStack.back()));
				controller.undoStack.pop_back();
			}
			return true;
		}

		//Back to front
		for (auto i = repeated_commands.rbegin(); i != repeated_commands.rend(); ++i) {
			(*i)->execute(); //Ignores failed commands
		}

		return true;
	}
	
	bool CMD_CollateCommands::undo()
	{
		//Front to back
		for (auto i = repeated_commands.begin(); i != repeated_commands.end(); ++i) {
			(*i)->undo(); //Ignores failed commands
		}

		return true;
	}

}
