#pragma once
#include <idk.h>
#include "Handle.h"

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
		template<typename T> Handle<T> GetComponent();
		template<typename T> bool      HasComponent();
		template<typename T> bool      RemoveComponent(const Handle<T>&);

		// active flags
		void SetActive(bool);
		bool GetActiveSelf() const;
		bool GetActiveInHierarchy() const;
	private:
		vector<GenericHandle> _components;

		GameObject(const GameObject&) = delete;
		GameObject& operator=(const GameObject&) = delete;
	};
}

#include "GameObject.inl"