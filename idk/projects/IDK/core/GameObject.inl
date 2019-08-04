#pragma once

#include "GameObject.h"
#include "Scene.h"

namespace idk
{

	template<typename T> 
	Handle<T> GameObject::AddComponent()
	{
		auto h = _scene->CreateObject<T>();
		_components.emplace_back(h);
		return h;
	}
	template<typename T>
	opt<Handle<T>> GameObject::GetComponent()
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
	template<typename T>
	bool GameObject::RemoveComponent(const Handle<T>& handle)
	{
		for (auto& elem : _components)
		{
			if (elem.id == handle.id)
			{
				assert(_scene->DestroyObject(handle));
				return true;
			}
		}
		return false;
	}
}