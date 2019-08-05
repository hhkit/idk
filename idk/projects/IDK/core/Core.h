#pragma once
#include <core/GameState.h>
#include <core/SystemManager.h>

namespace idk
{
	class Core
	{
	public:
		template<typename T>   static T& GetSystem();
		template<typename App> static Core MakeCore();

		Core(std::shared_ptr<Application>&& app, std::shared_ptr<IEditor>&& editor = std::shared_ptr<IEditor>{});
		void Run();
	private:
		SystemManager _system_manager;
		//ResourceManager _resource_manager; // let's try not to overengineer this one
		//Scheduler   _scheduler;            // or this one
		GameState     _game_state;
		static inline Core* _instance = nullptr;
	};
}
#include "Core.inl"