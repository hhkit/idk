//////////////////////////////////////////////////////////////////////////////////
//@file		CommandController.cpp
//@author	Muhammad Izha B Rahim
//@param	Email : izha95\@hotmail.com
//@date		04 OCT 2019
//@brief	

/*
Handles Command objects and stores them for undo and redo
The purpose of this class is to NOT know what type of command it is or does.
It is to only handle the interface given.
Takes in a NEW Commands and handles its delete internally

*/
//////////////////////////////////////////////////////////////////////////////////


#include "pch.h"
#include "CommandController.h"
#include <editor/commands/CMD_DeleteGameObject.h>
#include <editor/commands/CMD_DeleteComponent.h>
#include <editor/commands/CMD_CollateCommands.h>
#include <IDE.h>

namespace idk 
{
	CommandController::CommandController()
	{
	}
	CommandController::~CommandController()
	{
	}

	ICommand* CommandController::ExecuteCommand(unique_ptr<ICommand> command)
	{
		if (command->execute())
		{
			redoStack.clear();
			undoStack.push_back(std::move(command));
			if (undoStack.size() > undoLimit) //If exceed limit, delete the last one
				undoStack.pop_front();
			return undoStack.back().get();
		}
		return nullptr;
	}

	void CommandController::FlushCommands()
	{
		//while (pollStack.size()) {
		//	
		//	unique_ptr<ICommand> command = std::move(pollStack.front());
		//	pollStack.pop();

		//	const bool isSuccess = command->execute();

		//	if (isSuccess) {
		//		if (undoStack.size() >= undoLimit) { //If exceed limit, delete the last one
		//			undoStack.pop_front();
		//		}

		//		undoStack.push_back(std::move(command));

		//		if (redoStack.size() != NULL) {     //Clear the redo stack after execution
		//			redoStack.clear();
		//		}
		//		if (dynamic_cast<CMD_DeleteGameObject*>(undoStack.back().get()))
		//			break; //Exit loop to safely delete gameobject till next frame. (This is so that CMD_DeleteGameObject does not remove an already deleted gameobject again. Important when undoing)
		//		if (dynamic_cast<CMD_DeleteComponent*>(undoStack.back().get()))
		//			break; //Exit loop to safely delete gameobject till next frame. (This is so that CMD_DeleteGameObject does not remove an already deleted gameobject again. Important when undoing)
		//		if (dynamic_cast<CMD_CollateCommands*>(undoStack.back().get()))
		//			break;
		//	}
		//}
	}

	void CommandController::UndoCommand()  //Does not handle deletes, just moves the pointer to another list
	{
		if (undoStack.empty())
			return;
		if (undoStack.back()->undo())  //If execute is a success, add to the redo stack
			redoStack.push_back(std::move(undoStack.back()));
		undoStack.pop_back();
	}

	void CommandController::RedoCommand() 
	{
		if (redoStack.empty())
			return;
		if (redoStack.back()->execute())  //If execute is a success, add to the undo stack
			undoStack.push_back(std::move(redoStack.back()));
		redoStack.pop_back();
	}

	void CommandController::ClearUndoRedoStack()
	{
		undoStack.clear();
		redoStack.clear();
	}

	bool CommandController::CanUndo()
	{
		return !undoStack.empty();
	}

	bool CommandController::CanRedo()
	{
		return !redoStack.empty();
	}


}