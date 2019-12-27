#pragma once
#include <idk_config.h>
#include "GameObject.h"
#include "GameState.h"
#include <ds/span.inl>
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
		RegisterComponent(comp);
		return comp;
	}
	template<typename T>
	Handle<T> GameObject::GetComponent() const
	{
		const component_range& component_set = _component_ranges[ComponentID<T>];
		
		return component_set.count ? handle_cast<T>(_components[component_set.begin]) : Handle<T>{};
	}
	template<typename T>
	inline span<const Handle<T>> GameObject::GetComponents() const
	{
		const component_range& component_set = _component_ranges[ComponentID<T>];
		auto beg = static_cast<const Handle<T>*>(_components.data() + component_set.begin);
		return span<const Handle<T>>(beg, beg + component_set.count);
	}
	template<typename T>
	inline bool GameObject::HasComponent() const
	{
		return _component_ranges[ComponentID<T>].count != 0;
	}

	template<typename Component>
	inline void GameObject::RegisterComponent(Handle<Component> component)
	{
		component_range& component_set = _component_ranges[ComponentID<Component>];
		_components.insert(_components.begin() + component_set.begin + component_set.count, component);
		++component_set.count;
		for (auto itr = _component_ranges.begin() + ComponentID<Component> +1; itr != _component_ranges.end(); ++itr)
		{
			component_range& set = *itr;
			++set.begin;
		}
	}
	template<typename Component>
	inline void GameObject::DeregisterComponent(Handle<Component> component)
	{
		component_range& component_set = _component_ranges[ComponentID<Component>];
		auto beg = _components.begin() + component_set.begin;
		// remove component
		{
			auto itr = std::find(beg, beg + component_set.count, GenericHandle{ component });
			IDK_ASSERT(itr != _components.end());
			_components.erase(itr);
			component_set.count--;
		}
		// update remaining ranges
		for (auto itr = _component_ranges.begin() + ComponentID<Component> +1; itr != _component_ranges.end(); ++itr)
		{
			component_range& set = *itr;
			--set.begin;
		}
	}
}