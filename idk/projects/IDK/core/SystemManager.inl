#pragma once
#include <idk.h>
#include <meta/meta.h>
#include "SystemManager.h"

namespace idk
{

	template<typename T>
	T& SystemManager::GetSystem()
	{
		return *s_cast<T*>(std::get<Helper::GetSystemImpl<T>()>(_list).get());
	}

	template<typename T, typename ...Args>
	inline T& SystemManager::AddSystem(Args&& ... args)
	{
		std::get<Helper::GetSystemImpl<T>()>(_list) = std::make_shared<T>(std::forward<Args>(args)...);
		return *s_cast<T*>(std::get<Helper::GetSystemImpl<T>()>(_list).get());
	}
}