//////////////////////////////////////////////////////////////////////////////////
//@file		CMD_DeleteComponent.h
//@author	Muhammad Izha B Rahim
//@param	Email : izha95\@hotmail.com
//@date		28 SEPT 2019
//@brief	Adds components to gameobjects
//////////////////////////////////////////////////////////////////////////////////


#include <editor/commands/ICommand.h>
#include <core/GameObject.h>

namespace idk {

	class CMD_DeleteComponent : public ICommand { //serialize/deserialize use text.h
	public:
		CMD_DeleteComponent(Handle<GameObject> gameObject, string componentName);

		virtual bool execute() override;

		virtual bool undo() override;

	private:

		GenericHandle		new_component_handle	{};
		const string		component_name			{};
		string				serialized_component	{};
	};

}