//////////////////////////////////////////////////////////////////////////////////
//@file		CMD_DeleteGameObject.h
//@author	Muhammad Izha B Rahim
//@param	Email : izha95\@hotmail.com
//@date		02 OCT 2019
//@brief	Delete gameobjects
//////////////////////////////////////////////////////////////////////////////////


#include <editor/commands/ICommand.h>
#include <core/GameObject.h>

namespace idk {


	class CMD_DeleteGameObject : public ICommand { //serialize/deserialize use serialize.h
	public:
		CMD_DeleteGameObject(Handle<GameObject> gameObject);

		virtual bool execute() override;

		virtual bool undo() override;

	private:

		void RecursiveCreateObjects(vector<RecursiveObjects>& vector_ref, bool isRoot = false);

		vector<RecursiveObjects>	gameobject_vector	{};
		vector<ICommand*>			commands_affected	{}; //Stores a dumb pointer (This is to check if the unique pointer is still there)
	};

}