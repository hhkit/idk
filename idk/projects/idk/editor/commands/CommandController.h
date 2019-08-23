//////////////////////////////////////////////////////////////////////////////////
//@file		CommandController.h
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

#pragma once

#include <list>
#include <editor/commands/ICommand.h>

#define DEFAULT_UNDOLIMIT 512 //Similarly, redo can go up to 512


namespace idk {

	class CommandController {
	private:

		//A vector of past commands that have been executed. 
		//When ExecuteCommand or RedoCommand is called, this is added to stack. 
		//When UndoCommand is called, this stack is popped off.
		//This stack is cleared when changing scenes.
		std::list<ICommand*> undoStack;


		//A vector of past commands that have been undone. 
		//When ExecuteCommand is called, this stack is cleared. 
		//When UndoCommand is called, this is added to stack.
		//This stack is cleared when changing scenes.
		std::list<ICommand*> redoStack;


		size_t undoLimit; //By default it is 512
	public:
		CommandController();
		~CommandController();

		//Executes given ICommand Object. USAGE: ExecuteCommand(new MoveObject{params}); DO NOT SEND IN LOCAL VARIABLES
		void ExecuteCommand(ICommand* command);
		//Executes the undo function of previous ICommand Object.
		void UndoCommand();
		//Exectues function of an undone ICommand Object.
		void RedoCommand();
		//Clears the stack
		void ClearUndoRedoStack();
	};

}