//////////////////////////////////////////////////////////////////////////////////
//@file		CMD_CreateGameObject.h
//@author	Muhammad Izha B Rahim
//@param	Email : izha95\@hotmail.com
//@date		10 SEPT 2019
//@brief	Adds components to gameobjects
//////////////////////////////////////////////////////////////////////////////////


#include <editor/commands/ICommand.h>
#include <core/GameObject.h>

namespace idk {

	class CMD_CreateGameObject : public ICommand { //serialize/deserialize use text.h
	public:
        CMD_CreateGameObject(Handle<GameObject> parenting_gameobject, string name = "", vector<string> initial_components = {}); //Create empty on parented
		CMD_CreateGameObject(vector<RecursiveObjects> copied_object = {}); //Create empty, if use_copy is true, will use IDE::copied_gameobject to generate gameobject

		virtual bool execute() override;

		virtual bool undo() override;

	private:
		Handle<GameObject> parenting_gameobject{};
        string name;
        vector<string> initial_components{};

		bool RecursiveCreateObjects(vector<RecursiveObjects>& vector_ref, bool isRoot = false);
		vector<RecursiveObjects> copied_object{}; //If this is empty, create empty. Else use copied_object to build gameobject
	};

}