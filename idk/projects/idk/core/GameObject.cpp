#include "stdafx.h"
#include "GameObject.inl"
#include <common/Transform.h>
#include <common/Name.h>
#include <common/TagManager.h>
#include <common/Tag.h>
#include <common/Layer.h>
#include <reflect/reflect.inl>
#include <ds/span.inl>
namespace idk
{
	GenericHandle GameObject::AddComponent(string_view sv)
	{
		return AddComponent(reflect::get_type(sv));
	}
	GenericHandle GameObject::AddComponent(reflect::type type)
	{
		return GameState::GetGameState().CreateComponent(GetHandle(), type);
	}
	GenericHandle GameObject::AddComponent(reflect::dynamic dyn)
	{
		return GameState::GetGameState().CreateComponent(GetHandle(), dyn);
	}
	GenericHandle GameObject::AddComponent(GenericHandle component_handle, reflect::dynamic dyn)
	{
		return GameState::GetGameState().CreateComponent(GetHandle(), component_handle, dyn);
	}
	bool GameObject::SetComponentIndex(GenericHandle component_handle, unsigned pos)
	{
		auto& range = _component_ranges[component_handle.type - 1];
		if (static_cast<unsigned>(range.count) <= pos) // out of range
			return false;

		// find the handle
		auto curr = 0U;
		for (; curr != static_cast<unsigned>(range.count); ++curr)
			if (_components[range.begin + curr] == component_handle)
				break;

		if (curr == static_cast<unsigned>(range.count)) // could not find handle
			return false;

		// if less than, we swap forward
		if (curr < pos)
		{
			while (curr != pos)
			{
				std::swap(_components[range.begin + curr], _components[range.begin + pos]);
				++curr;
			}
		}
		else // swap backward
		if (curr > pos)
		{
			while (curr != pos)
			{
				std::swap(_components[range.begin + curr], _components[range.begin + pos]);
				--curr;
			}
		}

		return true;
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
        if (const auto tag = GetComponent<class Tag>())
            return Core::GetSystem<TagManager>().GetTagFromIndex(tag->index);
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
            tag_c->index = Core::GetSystem<TagManager>().GetIndexFromTag(tag);
        }
    }

    char GameObject::Layer() const
    {
        if (const auto layer = GetComponent<class Layer>())
            return layer->index;
        return 0;
    }

    void GameObject::Layer(char layer)
    {
        AddComponent<class Layer>()->index = layer;
    }
}
