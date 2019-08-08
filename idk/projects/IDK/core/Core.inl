#include "Core.h"
#pragma once
namespace idk
{
	template<typename T> T& Core::GetSystem()
	{
		return Core::_instance->_system_manager.GetSystem<T>();
	}
	template<typename T, typename ...Args>
	inline T& Core::AddSystem(Args&& ...args)
	{
		return _system_manager.AddSystem<T>(std::forward<Args>(args)...);
	}
}