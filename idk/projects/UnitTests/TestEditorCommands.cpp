//////////////////////////////////////////////////////////////////////////////////
//@file		TestEditorCommands.cpp
//@author	Muhammad Izha B Rahim
//@param	Email : izha95\@hotmail.com
//@date		12 AUG 2019
//@brief	Unit test Commands. All these classes will be moved to separate editor classes once tested.
//////////////////////////////////////////////////////////////////////////////////


#include "pch.h" // gtest.h
#include <core/Handle.h>
#include <reflect/reflect.h>
#include <IncludeComponents.h> //Gameobject, transform

namespace idk {
	class ICommandTest {
	public:
		//Disables Copy or Move constructor
		ICommandTest() {}
		ICommandTest(ICommandTest&& disabled) = delete;
		ICommandTest(const ICommandTest& disabled) = delete;
		virtual bool execute() = 0;							//Return true if the command works. When it is true, it will add to the undo stack

		virtual bool undo() = 0;							//Return true if the command works. When it is true, it will add to the redo stack
		virtual ~ICommandTest() = default;					// virtual needs default destructor
	};


	class Cmd_AddComponent : public ICommandTest{ //serialize/deserialize use serialize.h
	public:
		Cmd_AddComponent(Handle<GameObject> gameObject, string component)
			:game_object_handle{gameObject}, component_name{ component }
		{}

		virtual bool execute() override {
			if (game_object_handle) {
				new_component_handle = game_object_handle->AddComponent(reflect::get_type(component_name));
				//Find all Commands of similar objects in the controller and modify the handle to point to this!
				return true;
			}
			return false;
		}				

		virtual bool undo() override {
			if (game_object_handle) {
				if (new_component_handle) {
					game_object_handle->RemoveComponent(new_component_handle);
					return true;
				}
				else {
					return false;
				}
			}

			return false;
		}		

	private:
		Handle<GameObject>	game_object_handle;
		GenericHandle		new_component_handle;
		const string		component_name;
	};

	class Cmd_MoveGameObject : public ICommandTest { //This command is saved when mouse is released, after moving.
	public:
		Cmd_MoveGameObject(Handle<GameObject> gameObject, const vec3& pos) :game_object_handle{ gameObject },position { pos } {}
		virtual bool execute() override {
			if (game_object_handle) {
				Handle<Transform> transformHandle = game_object_handle->GetComponent<Transform>();
				if (transformHandle) {
					transformHandle->position += position;
					return true;
				}
				else {
					return false;
				}
			}
			return false;
		}

		virtual bool undo() override {
			if (game_object_handle) {
				Handle<Transform> transformHandle = game_object_handle->GetComponent<Transform>();
				if (transformHandle) {
					transformHandle->position -= position;
					return true;
				}
				else {
					return false;
				}
			}
			return false;
		}

	private:
		Handle<GameObject>	game_object_handle;
		const vec3			position;
	};

	class Cmd_RotateGameObject : public ICommandTest { //This command is saved when mouse is released, after moving.
	public:
		Cmd_RotateGameObject(Handle<GameObject> gameObject, const quat& rot): game_object_handle{ gameObject }, rotation{ rot } {}
		virtual bool execute() override {
			//Do rotations
		}

		virtual bool undo() override {
			//Undo rotations
		}

	private:
		Handle<GameObject>	game_object_handle;
		const quat			rotation;
	};

	class Cmd_ScaleGameObject : public ICommandTest { //This command is saved when mouse is released, after moving.
	public:
		Cmd_ScaleGameObject(Handle<GameObject> gameObject, const vec3& sca) : game_object_handle{ gameObject }, scale{ sca } {}
		virtual bool execute() override {
			if (game_object_handle) {
				Handle<Transform> transformHandle = game_object_handle->GetComponent<Transform>();
				if (transformHandle) {
					transformHandle->scale += scale;
					return true;
				}
				else {
					return false;
				}
			}
			return false;
		}

		virtual bool undo() override {
			if (game_object_handle) {
				Handle<Transform> transformHandle = game_object_handle->GetComponent<Transform>();
				if (transformHandle) {
					transformHandle->scale -= scale;
					return true;
				}
				else {
					return false;
				}
			}
			return false;
		}

	private:
		Handle<GameObject>	game_object_handle;
		const vec3			scale;
	};





	class CommandControllerTest {
	public:
		CommandControllerTest() {}
		~CommandControllerTest() {
			ClearUndoRedoStack();
		}

		//Executes given ICommand Object. USAGE: ExecuteCommand(std::make_unique<CommandName>(parameters)); DO NOT SEND IN LOCAL VARIABLES
		void ExecuteCommand(unique_ptr<ICommandTest> command) {
			bool isSuccess = command->execute();

			if (isSuccess) {
				if (undoStack.size() >= undoLimit) { //If exceed limit, delete the last one
					undoStack.pop_front();
				}
				undoStack.push_back(std::move(command));


				if (redoStack.size() != NULL) {     //Clear the redo stack after execution
					redoStack.clear();
				}
			}
		}
		//Executes the undo function of previous ICommand Object.
		void UndoCommand() {
			if (undoStack.size() == NULL)
				return;

			if (undoStack.back()->undo())  //If execute is a success, add to the redo stack
				redoStack.push_back(std::move(undoStack.back()));

			undoStack.pop_back();
		}
		//Exectues function of an undone ICommand Object.
		void RedoCommand() {
			if (redoStack.size() == NULL)
				return;

			if (redoStack.back()->execute())  //If execute is a success, add to the undo stack
				undoStack.push_back(std::move(redoStack.back()));


			redoStack.pop_back();
		}
		//Clears the stack
		void ClearUndoRedoStack() {
			if (undoStack.size() != NULL) {     //Clear the redo stack after execution
				undoStack.clear();
			}

			if (redoStack.size() != NULL) {     //Clear the redo stack after execution
				redoStack.clear();
			}
		}

	private:

		//A vector of past commands that have been executed. 
		//When ExecuteCommand or RedoCommand is called, this is added to stack. 
		//When UndoCommand is called, this stack is popped off.
		//This stack is cleared when changing scenes.
		std::list<unique_ptr<ICommandTest>> undoStack{};


		//A vector of past commands that have been undone. 
		//When ExecuteCommand is called, this stack is cleared. 
		//When UndoCommand is called, this is added to stack.
		//This stack is cleared when changing scenes.
		std::list<unique_ptr<ICommandTest>> redoStack{};


		size_t undoLimit{ 512 }; //By default it is 512

	};










}

TEST(Editor, Commands)
{
	//Need to init a gamestate before testing
	using namespace idk;
	GameState gameState;
	CommandControllerTest commandControllerTest;

	gameState.ActivateScene(0);
	auto h = gameState.CreateObject<GameObject>(0);
	std::cout<< "Num of components: "<< h->GetComponents().size() << "\n" ;
	for (auto& elem : h->GetComponents())
		std::cout << elem.id << "\n";

	commandControllerTest.ExecuteCommand(std::make_unique<Cmd_AddComponent>(h,"Transform"));
	commandControllerTest.ExecuteCommand(std::make_unique<Cmd_AddComponent>(h,"Transform"));
	commandControllerTest.ExecuteCommand(std::make_unique<Cmd_AddComponent>(h,"Transform"));
	commandControllerTest.ExecuteCommand(std::make_unique<Cmd_AddComponent>(h,"Transform"));
	commandControllerTest.ExecuteCommand(std::make_unique<Cmd_AddComponent>(h,"Transform"));
	commandControllerTest.ExecuteCommand(std::make_unique<Cmd_AddComponent>(h,"Transform"));
	std::cout << "Num of components: " << h->GetComponents().size() << "\n";
	for (auto& elem : h->GetComponents())
		std::cout << elem.id << "\n";
	commandControllerTest.UndoCommand();
	commandControllerTest.UndoCommand();
	commandControllerTest.UndoCommand();
	gameState.DestroyQueue();
	std::cout << "Num of components: " << h->GetComponents().size() << "\n";
	for (auto& elem : h->GetComponents())
		std::cout << elem.id << "\n";


}

