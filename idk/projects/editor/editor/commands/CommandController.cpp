//////////////////////////////////////////////////////////////////////////////////
//@file		CommandController.cpp
//@author	Muhammad Izha B Rahim
//@param	Email : izha95\@hotmail.com
//@date		23 AUG 2019
//@brief	

/*
Handles Command objects and stores them for undo and redo
The purpose of this class is to NOT know what type of command it is or does.
It is to only handle the interface given.
Takes in a NEW Commands and handles its delete internally

*/
//////////////////////////////////////////////////////////////////////////////////


#include "stdafx.h"
#include <editor/commands/CommandController.h>

namespace idk {
	CommandController::CommandController()
	{
	}


	CommandController::~CommandController() {
		ClearUndoRedoStack();
	}

	void CommandController::ExecuteCommand(ICommand* command) {

		bool isSuccess = command->execute();

		if (isSuccess) {
			if (undoStack.size() >= undoLimit) { //If exceed limit, delete the last one
				delete undoStack.front();
				undoStack.pop_front();
			}
			undoStack.push_back(command);


			if (redoStack.size() != NULL) {     //Clear the redo stack after execution
				for (ICommand* i : redoStack) {
					delete i;
				}
				redoStack.clear();
			}
		}
	}

	void CommandController::UndoCommand() {  //Does not handle deletes, just moves the pointer to another list
		if (undoStack.size() == NULL)
			return;

		if (undoStack.back()->undo())  //If execute is a success, add to the redo stack
			redoStack.push_back(undoStack.back());

		undoStack.pop_back();
	}

	void CommandController::RedoCommand() {
		if (redoStack.size() == NULL)
			return;

		if (redoStack.back()->execute())  //If execute is a success, add to the undo stack
			undoStack.push_back(redoStack.back());


		redoStack.pop_back();

	}

	void CommandController::ClearUndoRedoStack()
	{
		if (undoStack.size() != NULL) {     //Clear the redo stack after execution
			for (ICommand* i : undoStack) {
				delete i;
			}
			undoStack.clear();
		}

		if (redoStack.size() != NULL) {     //Clear the redo stack after execution
			for (ICommand* i : redoStack) {
				delete i;
			}
			redoStack.clear();
		}
	}

}