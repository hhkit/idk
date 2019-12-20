#include "Core.h"
#pragma once
namespace idk
{
	template<typename T> T& Core::GetSystem()
	{
		return Core::_instance->GetSystemManager().GetSystem<T>();
	}
	template<typename T, typename ...Args>
	inline T& Core::AddSystem(Args&& ...args)
	{
		return GetSystemManager().AddSystem<T>(std::forward<Args>(args)...);
	}
}