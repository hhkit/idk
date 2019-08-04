#pragma once
#include <memory>

#include <idk.h>
#include <core/ObjectPool.h>
#include <core/Components.h>

namespace idk::detail
{
	template<typename T>
	struct ObjectPoolHelper;

	template<typename ... Ts>
	struct ObjectPoolHelper<tuple<Ts...>>
	{
		using Tuple = tuple<Ts...>;

		template<typename T>
		using storage = shared_ptr<idk::ObjectPool<T>>;

		using type = array<shared_ptr<void>, sizeof...(Ts)>;

		static auto Instantiate()
		{
			return type{
				std::make_shared< idk::ObjectPool<Ts>>()...
			};
		}

		static bool ActivateScene(type& pools, uint8_t index)
		{
			return ((s_cast<idk::ObjectPool<Ts>*>(pools[detail::index_in_tuple_v<Ts, Tuple>].get())->ActivateScene(index)) && ...);
		}

		static bool DeactivateScene(type& pools, uint8_t index)
		{
			return ((s_cast<idk::ObjectPool<Ts>*>(pools[detail::index_in_tuple_v<Ts, Tuple>].get())->DeactivateScene(index)) && ...);
		}

		template<typename T>
		static ObjectPool<T>& GetPool(type& pools)
		{
			return *s_cast<ObjectPool<T>*>(pools[detail::index_in_tuple_v<T, Tuple>].get());
		}
	};

	using ObjectPools = ObjectPoolHelper<Handleables>;
	using ObjectPools_t = ObjectPools::type;
}