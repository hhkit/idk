//////////////////////////////////////////////////////////////////////////////////
//@file		CommandController.h
//@author	Muhammad Izha B Rahim
//@param	Email : izha95\@hotmail.com
//@date		04 OCT 2019
//@brief	

/*
Handles Command objects and stores them for undo and redo
The purpose of this class is to NOT know what type of command it is or does.
It is to only handle the interface given.
Takes in a NEW Commands and handles its delete internally

NOTES:
To change list to circular buffer soon-ish.

*/
//////////////////////////////////////////////////////////////////////////////////

#pragma once

#include <list>
#include <queue>
#include <editor/commands/ICommand.h>

#define COMMAND(COMMAND_CLASSNAME,...) std::make_unique<COMMAND_CLASSNAME>(__VA_ARGS__)

namespace idk {

	class CommandController {
	public:
		CommandController();
		~CommandController();
		
		//Executes given ICommand Object. USAGE: ExecuteCommand(COMMAND(CMD_AddComponent,gameobjectHandle));
		//Puts all command into a stack that is called when flushcommands is called
		void ExecuteCommand(unique_ptr<ICommand> command);
		//Executes the undo function of previous ICommand Object.
		void UndoCommand();
		//Exectues function of an undone ICommand Object.
		void RedoCommand();
		//Clears the stack
		void ClearUndoRedoStack();

		bool CanUndo();

		bool CanRedo();
	private:
		friend class IDE;
		friend class CMD_DeleteGameObject;
		friend class CMD_DeleteComponent;

		//FlushCommand gathers all command call and this pollstack executes all at once at the end of IDE frame.
		void FlushCommands();

		//A vector of past commands that have been executed. 
		//When ExecuteCommand or RedoCommand is called, this is added to stack. 
		//When UndoCommand is called, this stack is popped off.
		//This stack is cleared when changing scenes.
		std::list<unique_ptr<ICommand>> undoStack{};

		//A vector of past commands that have been undone. 
		//When ExecuteCommand is called, this stack is cleared. 
		//When UndoCommand is called, this is added to stack.
		//This stack is cleared when changing scenes.
		std::list<unique_ptr<ICommand>> redoStack{};

		std::queue<unique_ptr<ICommand>> pollStack{}; //ExecuteCommand gathers all command call and this pollstack executes all at once at the end of IDE frame.

		size_t undoLimit			{ 512 }; //By default it is 512

	};

}