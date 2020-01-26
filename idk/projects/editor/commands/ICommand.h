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

#include <core/GameObject.h>
#include <reflect/reflect.h>

namespace idk 
{
	//This is for collecting deleted gameobjects and its children as well as CreateGameObject Used for undo
	struct RecursiveObjects {
		Handle<GameObject>			original_handle			{};
		Handle<GameObject>			parent_of_children		{}; //Only for children, used when undoing. The main deleted gameobject would have this as null.
		vector<reflect::dynamic>	vector_of_components	{}; //Contains components for the new gameobject
		vector<RecursiveObjects>	children				{};
	};

	class ICommand 
	{
	public:
		//Disables Copy or Move constructor
		ICommand() noexcept								= default;
		ICommand(const ICommand& disabled)				= delete;
		ICommand(ICommand&& disabled)					= delete;
		ICommand& operator=(const ICommand& disabled)	= delete;
		ICommand& operator=(ICommand&& disabled)		= delete;

		virtual bool execute() = 0;						//Return true if the command works. When it is true, it will add to the undo stack
		virtual bool undo() = 0;						//Return true if the command works. When it is true, it will add to the redo stack

		virtual ~ICommand() noexcept = default;			// virtual needs default destructor

		Handle<GameObject> GetGameObject() { return game_object_handle; }

	protected:
		//This is used in most commands. This is required by DeleteGameObject command as it checks and modifies this handle when calling undo.
		Handle<GameObject>	game_object_handle{};
		friend class CMD_DeleteGameObject;
	};
}