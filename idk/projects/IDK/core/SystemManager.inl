#pragma once
#include <idk.h>
#include <meta/meta.h>

namespace idk
{

	template<typename T>
	T& SystemManager::GetSystem()
	{
		return *s_cast<T*>(std::get<index_in_tuple_v<T, Systems>>(_list).get());
	}
}