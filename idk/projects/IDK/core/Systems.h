#pragma once
#include <tuple>
#include <core/ISystem.h>
#include <util/meta.h>
namespace idk
{
	class EditorSystem;

	using EngineSystems = std::tuple<
		class TestSystem
	//,	class InputSystem
	//,	class ScriptSystem
	//,	class PhysicsSystem
	//,	class GraphicsSystem
	//,	class AudioSystem
	//,	class EditorSystem
	>;

	using Systems = decltype(std::tuple_cat(
		std::declval<std::tuple<class Application>>(),
		std::declval<EngineSystems>(),
		std::declval<std::tuple<EditorSystem>>()
	));

	constexpr auto SystemCount = std::tuple_size_v<Systems>;

	namespace detail
	{
		template<typename T>
		struct SystemHelper;

		template<typename ...Ts>
		struct SystemHelper<std::tuple<Ts...>>
		{
			static auto InstantiateEngineSystems(shared_ptr<Application>&& app_system, shared_ptr<EditorSystem>&& editor_system)
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