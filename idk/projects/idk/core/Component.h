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
	
	template<typename T, bool UniqueC>
	class Component 
		: public Handleable<T>
	{
	public:
		static constexpr auto Unique = UniqueC;
		Handle<GameObject> GetGameObject() const { return _gameObject; };
		const Handle<T>& GetHandle() const noexcept { return Handleable<T>::GetHandle(); }

		static_assert(index_in_tuple_v<T, Components> != std::tuple_size_v<Components>, "T must be a Component in idk_config.h/Components");
	private:

		Handle<GameObject> _gameObject;
		friend class GameObject;
		template<typename> friend struct detail::TableGenerator;
	};
}