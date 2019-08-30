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
		template<typename T> bool      RemoveComponent(const Handle<T>&);

		// generic
		GenericHandle AddComponent(reflect::type);
		GenericHandle AddComponent(reflect::dynamic);
		GenericHandle GetComponent(reflect::type);
		GenericHandle GetComponent(string_view);
		void          RemoveComponent(GenericHandle);

		span<GenericHandle> GetComponents();
		// active flags
		void SetActive(bool);
		bool ActiveSelf() const;
		bool ActiveInHierarchy() const;

		// components
		Handle<class Transform> Transform();
		Handle<class GameObject> ParentObject();
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