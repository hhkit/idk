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
		vector<reflect::dynamic>	vector_of_components{};


	};

}