#include "stdafx.h"
#include "GameObject.h"
#include <common/Transform.h>
#include <common/Name.h>
#include <common/TagSystem.h>
#include <common/Tag.h>

namespace idk
{
	GenericHandle GameObject::AddComponent(reflect::type type)
	{
		return GameState::GetGameState().CreateComponent(GetHandle(), type);
	}
	GenericHandle GameObject::AddComponent(reflect::dynamic dyn)
	{
		return GameState::GetGameState().CreateComponent(GetHandle(), dyn);
	}
	GenericHandle GameObject::GetComponent(reflect::type type)
	{
		const auto tid = GameState::GetGameState().GetTypeID(type);

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
	span<GenericHandle> GameObject::GetComponents() noexcept
	{
		return span<GenericHandle>(_components);
	}
	void GameObject::SetActive(bool active) noexcept
	{
		_active = active;
	}
	bool GameObject::ActiveSelf() const noexcept
	{
		return _active;
	}
	bool GameObject::ActiveInHierarchy() const
	{
		const auto hParent = Transform()->parent;
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
		return IsQueuedForDestruction() || ParentIsQueuedForDestruction();
	}

	bool GameObject::ParentIsQueuedForDestruction() const
	{
		const auto parent = Parent();
		if (parent)
			return parent->HierarchyIsQueuedForDestruction();
		else
			return false;
	}

	string_view GameObject::Name() const
	{
		return GetComponent<class Name>()->name;
	}

	void GameObject::Name(string_view name)
	{
		GetComponent<class Name>()->name = name;
	}

    string_view GameObject::Tag() const
    {
        const auto tag = GetComponent<class Tag>();
        if (tag)
            return Core::GetSystem<TagSystem>().GetTagFromIndex(tag->index);
        else
            return "";
    }

    void GameObject::Tag(string_view tag)
    {
        auto tag_c = GetComponent<class Tag>();
        if (tag.empty())
            RemoveComponent(tag_c);
        else
        {
            if (!tag_c)
                tag_c = AddComponent<class Tag>();
            tag_c->index = Core::GetSystem<TagSystem>().GetIndexFromTag(tag);
        }
    }
}
