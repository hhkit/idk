#pragma once
#include <memory>

#include <idk.h>
#include <core/ObjectPool.h>
#include <event/Signal.h>

namespace idk
{
	class GameState;
}
namespace idk::detail
{
	template<typename T>
	struct TableGenerator;

	template<typename T>
	struct ObjectPoolHelper;

	template<typename ... Ts>
	struct ObjectPoolHelper<std::tuple<Ts...>>
	{
		using Tuple = std::tuple<Ts...>;

		template<typename T>
		using storage = shared_ptr<idk::ObjectPool<T>>;

		static constexpr auto TypeCount = sizeof...(Ts);
		using type = array<shared_ptr<void>, sizeof...(Ts)>;

		template<typename T>
		static ObjectPool<T>& GetPool(type& pools) noexcept
		{
			return *s_cast<ObjectPool<T>*>(pools[index_in_tuple_v<T, Tuple>].get());
		}

		template<typename T>
		struct handleable_signal
		{
			using type = Signal<Handle<T>>;
		};

		using SignalTuple = std::tuple<Signal<Handle<Ts>>...>;
	};

	using ObjectPools = ObjectPoolHelper<Handleables>;
	using ObjectPools_t = ObjectPools::type;
}