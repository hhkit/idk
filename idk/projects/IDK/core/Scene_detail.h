#pragma once
#include <memory>

#include <core/ObjectPool.h>
#include <core/Handleables.h>

namespace idk::detail
{
	template<typename T>
	struct ScenePoolHelper;

	template<typename ... Ts>
	struct ScenePoolHelper<tuple<Ts...>>
	{
		using type = tuple<
			std::unique_ptr<idk::ObjectPool<Ts>>...
		>;

		static auto Instantiate(uint8_t build_index)
		{
			return type{
				std::make_unique< idk::ObjectPool<Ts>>(build_index)...
			};
		}
	};

	using ScenePool = ScenePoolHelper<Handleables>;
	using ScenePool_t = ScenePoolHelper<Handleables>::type;
}