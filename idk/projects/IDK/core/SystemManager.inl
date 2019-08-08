#pragma once
#include <idk.h>
#include <meta/meta.h>

namespace idk
{

	template<typename T>
	T& SystemManager::GetSystem()
	{
		return *s_cast<T*>(std::get<Helper::GetSystemImpl<T>()>(_list).get());
	}
}