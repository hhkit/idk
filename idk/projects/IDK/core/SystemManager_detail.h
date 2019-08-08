#pragma once
#include <tuple>
#include <idk_config.h>
#include <core/ISystem.h>
#include <meta/meta.h>
namespace idk
{
	class IEditor;

	namespace detail
	{
		template<typename T>
		struct SystemHelper;

		template<typename ...Ts>
		struct SystemHelper<std::tuple<Ts...>>
		{
			static auto InstantiateEngineSystems()
			{
				return std::array<shared_ptr<ISystem>, SystemCount>{
					[]()
					{
						if constexpr (std::is_abstract_v<Ts>)
							return std::shared_ptr<Ts>();
						else
							return std::make_shared<Ts>();
					}()...
				};
			}

			template<typename T, unsigned N, typename Query, typename ... Tail>
			static constexpr auto GetSystemIndex()
			{
				if constexpr (std::is_base_of_v<Query, T>)
					return N;
				else
					return GetSystemIndex<T, N + 1, Tail...>();
			}

			template<typename T>
			static constexpr auto GetSystemImpl()
			{
				return GetSystemIndex<T, 0, Application, Ts..., IEditor>();
			}
		};
	}
}