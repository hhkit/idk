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
			static auto InstantiateEngineSystems(shared_ptr<Application>&& app_system, shared_ptr<IEditor>&& editor_system)
			{
				return std::array<shared_ptr<ISystem>, SystemCount>{
					std::move(app_system),
					std::make_shared<Ts>()...,
					std::move(editor_system)
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