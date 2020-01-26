//////////////////////////////////////////////////////////////////////////////////
//@file		CMD_SelectGameObject.cpp
//@author	Muhammad Izha B Rahim
//@param	Email : izha95\@hotmail.com
//@date		03 DEC 2019
//@brief	Select gameobjects
//////////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "CMD_SelectObject.h"
#include <IDE.h>

namespace idk 
{
	CMD_SelectObject::CMD_SelectObject(ObjectSelection new_selection)
		: old_selection{ Core::GetSystem<IDE>().GetSelectedObjects() }, new_selection{ new_selection }
	{
	}

	bool CMD_SelectObject::execute()
	{
		Core::GetSystem<IDE>()._selected_objects = new_selection;
		return true;
	}

	bool CMD_SelectObject::undo()  
	{
		Core::GetSystem<IDE>()._selected_objects = old_selection;
		return true;
	}
}
