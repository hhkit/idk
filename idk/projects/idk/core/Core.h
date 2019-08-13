#pragma once
#include <core/GameState.h>
#include <core/SystemManager.h>
#include <core/FileSystem.h>
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
		void Run();
	private:
		SystemManager               _system_manager;
		unique_ptr<class Scheduler> _scheduler;
		GameState                   _game_state;
		atomic<bool>                _running;
		static inline Core*         _instance = nullptr;
	};
}
#include "Core.inl"