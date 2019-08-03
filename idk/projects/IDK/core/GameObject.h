#pragma once
#include <idk.h>
#include "ObjectHandle.h"

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
		template<typename T> ObjectHandle<T> AddComponent();
		template<typename T> ObjectHandle<T> GetComponent();
		template<typename T> void            RemoveComponent(const ObjectHandle<T>&);

		// active flags
		bool SetActive();
		bool GetActiveSelf() const;
		bool GetActiveInHierarchy() const;
	private:
		vector<GenericHandle> _components;

		GameObject(const GameObject&) = delete;
		GameObject& operator=(const GameObject&) = delete;
	};
}