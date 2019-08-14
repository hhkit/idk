#pragma once
#include <idk.h>
#include "Handle.h"
#include "Scene.h"

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
		template<typename T> bool      RemoveComponent(const Handle<T>&);

		// generic
		GenericHandle AddComponent(reflect::type);

		span<GenericHandle> GetComponents();
		// active flags
		void SetActive(bool);
		bool GetActiveSelf() const;
		bool GetActiveInHierarchy() const;
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