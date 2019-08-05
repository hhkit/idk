#include "stdafx.h"
#include "GameObject.h"
#include <common/Parent.h>

namespace idk
{
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
