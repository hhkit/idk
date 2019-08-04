#pragma once
#include <memory>

#include <core/ObjectPool.h>
#include <core/Components.h>

namespace idk::detail
{
	template<typename T>
	struct ObjectPoolHelper;

	template<typename ... Ts>
	struct ObjectPoolHelper<tuple<Ts...>>
	{
		using type = tuple<
			std::unique_ptr<idk::ObjectPool<Ts>>...
		>;

		static auto Instantiate()
		{
			return type{
				std::make_unique< idk::ObjectPool<Ts>>()...
			};
		}
	};

	using ObjectPools = ObjectPoolHelper<Components>;
	using ObjectPools_t = ObjectPools::type;
}