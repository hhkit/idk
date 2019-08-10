#pragma once
#include <idk.h>
#include "Handle.h"
#include "Scene.h"

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
		template<typename T> Handle<T> AddComponent();
		template<typename T> Handle<T> GetComponent() const;
		template<typename T> bool      HasComponent() const;
		template<typename T> bool      RemoveComponent(const Handle<T>&);

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
	};
}

#include "GameObject.inl"