#pragma once
#include <memory>

#include <idk.h>
#include <core/ObjectPool.h>
#include <core/Components.h>

namespace idk
{
	class GameState;
}
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

		static constexpr auto TypeCount = sizeof...(Ts);
		using type = array<shared_ptr<void>, sizeof...(Ts)>;

		static auto Instantiate()
		{
			return type{
				std::allocate_shared< idk::ObjectPool<Ts>>(std::allocator<idk::ObjectPool<Ts>>{})...
			};
		}

		static bool ActivateScene(type& pools, uint8_t index)
		{
			return ((s_cast<idk::ObjectPool<Ts>*>(pools[index_in_tuple_v<Ts, Tuple>].get())->ActivateScene(index)) && ...);
		}

		static bool DeactivateScene(type& pools, uint8_t index)
		{
			return ((s_cast<idk::ObjectPool<Ts>*>(pools[index_in_tuple_v<Ts, Tuple>].get())->DeactivateScene(index)) && ...);
		}

		template<typename T>
		static ObjectPool<T>& GetPool(type& pools)
		{
			return *s_cast<ObjectPool<T>*>(pools[index_in_tuple_v<T, Tuple>].get());
		}

		static auto GenValidateJt()
		{
			return GameState::ValidateJT{
				[](GameState& gs, const GenericHandle& handle) -> bool
				{
					return gs.ValidateHandle(handle_cast<Ts>(handle));
				} ...
			};
		}

		static auto GenDestructionJt()
		{
			return GameState::DestroyJT{
				[](GameState& gs, const GenericHandle& handle)
				{
					if constexpr(std::is_same_v<Ts, GameObject>)
						gs.DestroyObjectNow(handle_cast<Ts>(handle));
					else
					{
						auto real_handle = handle_cast<Ts>(handle);
						if (real_handle)
						{
							auto entity = real_handle->GetGameObject();
							gs.DestroyObjectNow(real_handle);
							assert(entity);
							auto realent = std::addressof(*entity);
							auto itr = std::find(realent->_components.begin(), realent->_components.end(), handle);
							if (itr != realent->_components.end())
								entity->_components.erase(itr);
						}
					}
				} ...
			};
		}
	};

	using ObjectPools = ObjectPoolHelper<Handleables>;
	using ObjectPools_t = ObjectPools::type;
}