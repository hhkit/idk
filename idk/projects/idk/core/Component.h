#pragma once

#include <idk.h>
#include "Handle.h"

namespace idk::detail
{
	template<typename T>
	struct TableGenerator;
}

namespace idk
{
	class GameObject;

	// all components must derive from this template
	template<typename T, bool UniqueComponent = true> 
	class Component;

	// don't derive from this
	class GenericComponent
	{
	public:
		Handle<GameObject> GetGameObject() const;
		virtual const GenericHandle& GetHandle() const = 0;

		GenericComponent(const GenericComponent&) = default;
		GenericComponent(GenericComponent&&) noexcept = default;
		GenericComponent& operator=(const GenericComponent&) = default;
		GenericComponent& operator=(GenericComponent&&) noexcept = default;
		virtual ~GenericComponent() = default;
	private:
		Handle<GameObject> _gameObject;
		friend class GameObject;
		template<typename>
		friend struct detail::TableGenerator;
		GenericComponent() = default;
		template<typename T, bool> friend class Component;
	};

	template<typename T, bool UniqueC>
	class Component 
		: public GenericComponent, public Handleable<T>
	{
	public:
		static constexpr auto Unique = UniqueC;

		const Handle<T>& GetHandle() const override
		{
			return Handleable<T>::GetHandle();
		}

		static_assert(index_in_tuple_v<T, Components> != std::tuple_size_v<Components>, "T must be a Component in idk_config.h/Components");
	};
}