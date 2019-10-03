//////////////////////////////////////////////////////////////////////////////////
//@file		ICommand.h
//@author	Muhammad Izha B Rahim
//@param	Email : izha95\@hotmail.com
//@date		23 AUG 2019
//@brief	

/*
Command Interface
All interfaces/abstract classes or structs generally should an 'I' prefix

This is an example of how to create commands
When duplicating it copies all the components as it create the gameobjects.
When undoing, it keeps track of the gameobject. If it is not deleted, delete it! Else do nothing.
Commands will never handle new or delete, it can only contain pointers

*/
//////////////////////////////////////////////////////////////////////////////////

#pragma once
namespace idk {
	class ICommand {
	public:
		//Disables Copy or Move constructor
		ICommand() {}
		ICommand(ICommand&& disabled) = delete;
		ICommand(const ICommand& disabled) = delete;
		virtual bool execute() = 0;						//Return true if the command works. When it is true, it will add to the undo stack

		virtual bool undo() = 0;						//Return true if the command works. When it is true, it will add to the redo stack
		virtual ~ICommand() = default;					// virtual needs default destructor
	protected:
		friend class CMD_DeleteGameObject;

		Handle<GameObject>	game_object_handle		{};	//This is used in most commands

	};
}