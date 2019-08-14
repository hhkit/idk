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
	bool GameObject::GetActiveSelf() const
	{
		return _active;
	}
	bool GameObject::GetActiveInHierarchy() const
	{
		auto hParentComponent = GetComponent<Parent>();
		auto hParent = hParentComponent ? hParentComponent->parent : Handle<GameObject>{};
		return GetActiveSelf() && (hParent ? hParent->GetActiveInHierarchy() : true);
	}
}
