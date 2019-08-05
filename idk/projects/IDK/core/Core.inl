#include "Core.h"
#pragma once
namespace idk
{
	template<typename T> T& Core::GetSystem()
	{
		return Core::_instance->_system_manager.GetSystem<T>();
	}

	template<typename Application>
	inline Core Core::MakeCore()
	{
		return Core{ std::make_shared<Application>() };
	}
}