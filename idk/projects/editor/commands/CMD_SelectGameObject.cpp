//////////////////////////////////////////////////////////////////////////////////
//@file		CMD_SelectGameObject.cpp
//@author	Muhammad Izha B Rahim
//@param	Email : izha95\@hotmail.com
//@date		03 DEC 2019
//@brief	Select gameobjects
//////////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include <editor/commands/CMD_SelectGameObject.h>
#include <IDE.h>				 //IDE

namespace idk {

	CMD_SelectGameObject::CMD_SelectGameObject(Handle<GameObject> gameObject)
	{
		game_object_handle = gameObject;
	}

	bool CMD_SelectGameObject::execute() {
		//Skips first execute
		if (!first_execute) {
			//current_selected_state = Core::GetSystem<IDE>().selected_gameObjects;
			//previous_selected_state = Core::GetSystem<IDE>().selected_gameObjects;
			//previous_selected_state.erase(std::remove(previous_selected_state.begin(), previous_selected_state.end(), game_object_handle), previous_selected_state.end()); //Remove the selected one from state
			first_execute = true;
			if (game_object_handle)
				return true;
			else
				return false;
		}

		if (game_object_handle) {
			//		auto& selected_gameObjects = Core::GetSystem<IDE>().selected_gameObjects;
			//		selected_gameObjects = current_selected_state;
			//		
			//		//Check integrity of selected gameobjects
			//		for (auto i = selected_gameObjects.begin(); i != selected_gameObjects.end(); ++i) {
			//		
			//			if (!(*i)) {//If this gameobject is deleted, remove from selected
			//				selected_gameObjects.erase(i);
			//				--i;
			//			}
			//		}

			Core::GetSystem<IDE>().selected_gameObjects.emplace_back(game_object_handle);

			Core::GetSystem<IDE>().RefreshSelectedMatrix();

			return true;
		}
		return false;
	}

	bool CMD_SelectGameObject::undo()  {
		if (game_object_handle) {
			for (auto counter = 0; counter < Core::GetSystem<IDE>().selected_gameObjects.size(); ++counter) { //Select and deselect
				if (game_object_handle == Core::GetSystem<IDE>().selected_gameObjects[counter]) {
					Core::GetSystem<IDE>().selected_gameObjects.erase(Core::GetSystem<IDE>().selected_gameObjects.begin() + counter);
					break;
				}
			}
			//		auto& selected_gameObjects = Core::GetSystem<IDE>().selected_gameObjects;
			//		
			//		selected_gameObjects = previous_selected_state;
			//		
			//		//Check integrity of selected gameobjects
			//		for (auto i = selected_gameObjects.begin(); i != selected_gameObjects.end(); ++i) {
			//		
			//			if (!(*i)) {//If this gameobject is deleted, remove from selected
			//				selected_gameObjects.erase(i);
			//				--i;
			//			}
			//		}

			return true;
		}
		return false;
	}


}
