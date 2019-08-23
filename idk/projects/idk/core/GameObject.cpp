#include "stdafx.h"
#include "GameObject.h"
#include <common/Parent.h>

namespace idk
{
	GenericHandle GameObject::AddComponent(reflect::type type)
	{
		auto comphandle = GameState::GetGameState().CreateComponent(GetHandle(), type);
		_components.emplace_back(comphandle);
		return comphandle;
	}
	GenericHandle GameObject::AddComponent(reflect::dynamic dyn)
	{
		auto comph = GameState::GetGameState().CreateComponent(GetHandle(), dyn);
		_components.emplace_back(comph);
		return comph;
	}
	span<GenericHandle> GameObject::GetComponents()
	{
		return span<GenericHandle>(_components.begin()._Ptr, _components.end()._Ptr);
	}
	void GameObject::SetActive(bool active)
	{
		_active = active;
	}
	bool GameObject::ActiveSelf() const
	{
		return _active;
	}
	bool GameObject::ActiveInHierarchy() const
	{
		auto hParentComponent = GetComponent<class Parent>();
		auto hParent = hParentComponent ? hParentComponent->parent : Handle<GameObject>{};
		return ActiveSelf() && (hParent ? hParent->ActiveInHierarchy() : true);
	}

	Handle<class Transform> GameObject::Transform()
	{
		return GetComponent<class Transform>();
	}
	Handle<class GameObject> GameObject::ParentObject()
	{
		auto hParentComponent = GetComponent<class Parent>();
		return hParentComponent ? hParentComponent->parent : Handle<class GameObject>{};
	}
}
