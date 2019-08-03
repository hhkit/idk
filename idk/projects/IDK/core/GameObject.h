#pragma once
#include <idk.h>
#include "ObjectHandle.h"

namespace idk
{
	class Scene;

	class GameObject 
		: public Handleable<GameObject>
	{
	public:
		GameObject(Scene* scene);
		GameObject(GameObject&&) = default;
		GameObject& operator=(GameObject&&) = default;
		~GameObject() = default;

		// components
		template<typename T> ObjectHandle<T>      AddComponent();
		template<typename T> opt<ObjectHandle<T>> GetComponent();
		template<typename T> bool                 RemoveComponent(const ObjectHandle<T>&);

		// active flags
		bool SetActive();
		bool GetActiveSelf() const;
		bool GetActiveInHierarchy() const;
	private:
		vector<GenericHandle> _components;
		Scene* _scene = nullptr;

		GameObject(const GameObject&) = delete;
		GameObject& operator=(const GameObject&) = delete;

		void SetScene(Scene* _scene);
		friend class Scene;
	};
}

#include "GameObject.inl"