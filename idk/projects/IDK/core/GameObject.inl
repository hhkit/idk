#pragma once

#include "GameObject.h"

namespace idk
{
	template<typename T>
	Handle<T> GameObject::GetComponent()
	{
		for (auto& elem : _components)
			if (elem.type == Handle<T>::type_id)
				return handle_cast<T>(elem);

		return {};
	}
	template<typename T>
	inline bool GameObject::HasComponent()
	{
		for (auto& elem : _components)
			if (elem.type == Handle<T>::type_id)
				return true;

		return {};
	}
}