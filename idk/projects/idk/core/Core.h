#pragma once
#include <core/GameState.h>
#include <core/SystemManager.h>

namespace idk
{
	class ResourceManager;

	class Core
	{
	public:
		template<typename T>   static T& GetSystem();
		template<typename T, typename ... Args> T& AddSystem(Args&& ... args);
		static seconds GetDT();
		static seconds GetRealDT();
		static ResourceManager& GetResourceManager();
		static void    Shutdown();

		Core();
		~Core();
		void Setup();
		void Run();
	private:
		GameState                   _game_state;
		SystemManager               _system_manager;
		unique_ptr<class Scheduler> _scheduler;
		atomic<bool>                _running;
		static inline Core*         _instance = nullptr;
		bool                        _setup = false;
	};
}
#include "Core.inl"