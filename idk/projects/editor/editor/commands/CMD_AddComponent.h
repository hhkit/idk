//////////////////////////////////////////////////////////////////////////////////
//@file		CMD_AddComponent.h
//@author	Muhammad Izha B Rahim
//@param	Email : izha95\@hotmail.com
//@date		30 AUG 2019
//@brief	Adds components to gameobjects
//////////////////////////////////////////////////////////////////////////////////


#include <editor/commands/ICommand.h>
#include <core/GameObject.h>

namespace idk {

	class CMD_AddComponent : public ICommand { //serialize/deserialize use serialize.h
	public:
		CMD_AddComponent(Handle<GameObject> gameObject, string component);

		virtual bool execute() override;

		virtual bool undo() override;

	private:
		Handle<GameObject>	game_object_handle		{};
		GenericHandle		new_component_handle	{};
		const string		component_name			{};
	};

}