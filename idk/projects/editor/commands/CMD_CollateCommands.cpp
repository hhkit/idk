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
#include <editor/commands/CMD_CreateGameObject.h>
#include <IDE.h>

namespace idk {


	CMD_CollateCommands::CMD_CollateCommands(int counts) : num_of_times_to_repeat{ counts }
	{
		
	}


	bool CMD_CollateCommands::execute()
	{

		if (num_of_times_to_repeat < 2)
			return false;	//Ignore this command when the callcommand is 1 or less
		//printf("CMD CCA Count: %d\n", num_of_times_to_repeat);
		if (!first_call) {		//Do not execute on first command call. But collect commands from controller. This collates commands to this class.
			
			CommandController& controller = Core::GetSystem<IDE>().command_controller;
			vector<Handle<GameObject>> tempSelected{};

			for (auto i = 0; i < num_of_times_to_repeat; ++i) {

				repeated_commands.push_back(std::move(controller.undoStack.back()));
				controller.undoStack.pop_back();


				if (CMD_CreateGameObject* ptr = dynamic_cast<CMD_CreateGameObject*>((*repeated_commands.rbegin()).get())) {
					tempSelected.push_back(ptr->GetGameObject());

				}
			}
			
			if (!tempSelected.empty()) {
				Core::GetSystem<IDE>().selected_gameObjects = tempSelected;

			}
			
			first_call = true;
			return true;			//Once returned, this will automatically push into the undostack
		}

		//Test if it is a CMD_CreateGameObject, so that we can set selected to all created objects in one shot.
		const bool isType_CreateGameObject = dynamic_cast<CMD_CreateGameObject*>((*repeated_commands.rbegin()).get()) == nullptr ? false : true;
		vector<Handle<GameObject>> tempSelected{};


		//Back to front
		for (auto i = repeated_commands.rbegin(); i != repeated_commands.rend(); ++i) {
			if ((*i)->execute()) {//Ignores failed commands
				if (isType_CreateGameObject) {
					//Because of how CreateGameObject is selected after creation, I can add it to the tempSelected from IDE without touching the CMD_CreateGameObject
					//CMD_CreateGameObject will cause selected_gameobjects to be one, but we'll add a check anyway
					
					if (Core::GetSystem<IDE>().selected_gameObjects.size() == 1) {
						tempSelected.push_back(Core::GetSystem<IDE>().selected_gameObjects[0]);
					}
					

				}

			}
		}

		if (isType_CreateGameObject) {
			Core::GetSystem<IDE>().selected_gameObjects = tempSelected;
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
