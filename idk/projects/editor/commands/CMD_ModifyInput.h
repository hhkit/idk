//////////////////////////////////////////////////////////////////////////////////
//@file		CMD_ModifyInput.h
//@author	Muhammad Izha B Rahim
//@param	Email : izha95\@hotmail.com
//@date		16 SEPT 2019
//@brief	Mainly for ImGui::InputTexts or floats, this is templated
//////////////////////////////////////////////////////////////////////////////////


#include <editor/commands/ICommand.h>
#include <core/GameObject.h>

namespace idk {
	template <typename T>
	class CMD_ModifyInput 
		: public ICommand { //serialize/deserialize use text.h
	public:
		CMD_ModifyInput(GenericHandle componentAffected, T* dataPtr, const T& dataModified);

		virtual bool execute() override;

		virtual bool undo() override;


	private:
		T previous_data;
		T new_data;
		GenericHandle component_handle;
		T* ptr_to_data; //This is invalid whenever Handle is nulled.
	};
}

#include <editor/commands/CMD_ModifyInput.inl>
