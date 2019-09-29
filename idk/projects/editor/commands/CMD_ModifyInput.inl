//////////////////////////////////////////////////////////////////////////////////
//@file		CMD_ModifyInput.inl
//@author	Muhammad Izha B Rahim
//@param	Email : izha95\@hotmail.com
//@date		16 SEPT 2019
//@brief	Mainly for ImGui::InputTexts or floats, this is templated
//////////////////////////////////////////////////////////////////////////////////

namespace idk {

	template<class T>
	CMD_ModifyInput<T>::CMD_ModifyInput(GenericHandle componentAffected,T* dataPtr, const T& dataModified)
	{
		component_handle = componentAffected;
		ptr_to_data = dataPtr;
		previous_data = *dataPtr;
		new_data = dataModified;
	}

	template<class T>
	bool CMD_ModifyInput<T>::execute()
	{
		if (component_handle) {
			*ptr_to_data = new_data;
			return true;
		}
		return false;
	}

	template<class T>
	bool CMD_ModifyInput<T>::undo()
	{
		if (component_handle) {
			*ptr_to_data = previous_data;
			return true;
		}
		return false;
	}



}
