#pragma once

#include "GameObject.h"
#include "GameState.h"
namespace idk
{
	template<typename T>
	inline Handle<T> GameObject::AddComponent()
	{
		if constexpr (T::Unique)
		{
			auto existing_handle = GetComponent<T>();
			if (existing_handle)
				return existing_handle;
		}

		auto comp = GameState::GetGameState().CreateObject<T>(GetHandle().scene);
		comp->_gameObject = GetHandle();
		_components.emplace_back(comp);
		return comp;
	}
	template<typename T>
	Handle<T> GameObject::GetComponent() const
	{
		for (auto& elem : _components)
			if (elem.type == Handle<T>::type_id)
				return handle_cast<T>(elem);

		return {};
	}
	template<typename T>
	inline bool GameObject::HasComponent() const
	{
		for (auto& elem : _components)
			if (elem.type == Handle<T>::type_id)
				return true;

		return {};
	}
}