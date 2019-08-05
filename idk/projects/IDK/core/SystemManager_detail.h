#pragma once
#include <tuple>
#include <idk_config.h>
#include <core/ISystem.h>
#include <util/meta.h>
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
		};
	}
}