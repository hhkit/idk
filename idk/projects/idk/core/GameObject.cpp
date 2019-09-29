#include "stdafx.h"
#include "GameObject.h"
#include <common/Transform.h>
#include <common/Name.h>
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
	GenericHandle GameObject::GetComponent(reflect::type type)
	{
		auto tid = GameState::GetGameState().GetTypeID(type);

		for (auto& elem : _components)
			if (elem.type == tid)
				return elem;

		return GenericHandle{};
	}
	GenericHandle GameObject::GetComponent(string_view sv)
	{
		return GetComponent(reflect::get_type(sv));
	}
	void GameObject::RemoveComponent(GenericHandle h)
	{
		GameState::GetGameState().DestroyObject(h);
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
		auto hParent = Transform()->parent;
		return ActiveSelf() && (hParent ? hParent->ActiveInHierarchy() : true);
	}

	Handle<class Transform> GameObject::Transform() const
	{
		return GetComponent<class Transform>();
	}
	Handle<class GameObject> GameObject::Parent() const
	{
		return Transform()->parent;
	}

	bool GameObject::HierarchyIsQueuedForDestruction() const
	{
		auto parent = Parent();
		return parent ? IsQueuedForDestruction() || parent->HierarchyIsQueuedForDestruction() : IsQueuedForDestruction();
	}

	string_view GameObject::Name() const
	{
		return GetComponent<class Name>()->name;
	}

	void GameObject::Name(string_view name)
	{
		GetComponent<class Name>()->name = name;
	}
}
