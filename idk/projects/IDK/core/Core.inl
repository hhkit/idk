#include "Core.h"
#pragma once
namespace idk
{
	template<typename T> T& Core::GetSystem()
	{
		return Core::_instance->_system_manager.GetSystem<T>();
	}

	template<typename Application, typename ... Args>
	inline Core Core::MakeCore(Args&& ... args)
	{
		return Core{ std::make_shared<Application>(std::forward<Args>(args)...) };
	}
}