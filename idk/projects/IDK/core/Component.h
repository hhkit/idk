#pragma once

#include <idk.h>
#include "Handle.h"

namespace idk
{
	class GameObject;

	template<typename T> 
	class Component;

	class GenericComponent
	{
	public:
		Handle<GameObject> GetGameObject();
		virtual const GenericHandle& GetHandle() = 0;

		GenericComponent(const GenericComponent&) = default;
		GenericComponent(GenericComponent&&) noexcept = default;
		GenericComponent& operator=(const GenericComponent&) = default;
		GenericComponent& operator=(GenericComponent&&) noexcept = default;
		virtual ~GenericComponent() = default;
	private:
		Handle<GameObject> _gameObject;
		friend class GameObject;
		GenericComponent() = default;
		template<typename T> friend class Component;
	};

	template<typename T>
	class Component 
		: public GenericComponent, public Handleable<T>
	{
	public:
		const Handle<T>& GetHandle() override
		{
			return Handleable<T>::GetHandle();
		}
	};
}