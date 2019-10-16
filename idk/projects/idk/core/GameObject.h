#pragma once
#include <idk.h>
#include "Handle.h"
#include <scene/Scene.h>

namespace idk::reflect
{
	class type;
}


namespace idk
{
	class GameObject 
		: public Handleable<GameObject>
	{
	public:
		GameObject() = default;
		GameObject(GameObject&&) = default;
		GameObject& operator=(GameObject&&) = default;
		~GameObject() = default;

		// components
		// exact
		template<typename T> Handle<T> AddComponent();
		template<typename T> Handle<T> GetComponent() const;
		template<typename T> bool      HasComponent() const;

		// generic
		GenericHandle AddComponent(string_view);
		GenericHandle AddComponent(reflect::type);
		GenericHandle AddComponent(reflect::dynamic);
		GenericHandle GetComponent(reflect::type);
		GenericHandle GetComponent(string_view);
		void          RemoveComponent(GenericHandle);

		span<GenericHandle> GetComponents() noexcept;
		// active flags
		void SetActive(bool) noexcept;
		bool ActiveSelf() const noexcept;
		bool ActiveInHierarchy() const;

		// components
		Handle<class Transform> Transform() const;
		Handle<class GameObject> Parent() const;
		bool HierarchyIsQueuedForDestruction() const;
		bool ParentIsQueuedForDestruction() const;

		string_view Name() const;
		void Name(string_view name);
        string_view Tag() const;
        void Tag(string_view tag);
        uint8_t Layer() const;
        void Layer(uint8_t layer);
		
		
	private:
		vector<GenericHandle> _components;
		bool _active = true;

		GameObject(const GameObject&) = delete;
		GameObject& operator=(const GameObject&) = delete;

		friend class GameState;

		template<typename T>
		friend struct detail::ObjectPoolHelper;

		template<typename T>
		friend struct detail::TableGenerator;
	};
}

#include "GameObject.inl"