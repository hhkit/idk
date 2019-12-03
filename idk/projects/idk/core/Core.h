#pragma once
#include <core/GameState.h>
#include <core/SystemManager.h>

namespace idk
{
	class Scheduler;
	class ResourceManager;
	namespace mt { class ThreadPool; }

	class Core
	{
	public:
		template<typename T>   static T& GetSystem();
		template<typename T, typename ... Args> T& AddSystem(Args&& ... args);
		static seconds GetDT();
		static seconds GetRealDT();
		static GameState& GetGameState() { return _instance->_game_state; }
		static Scheduler& GetScheduler();
		static mt::ThreadPool& GetThreadPool();
		static ResourceManager& GetResourceManager();
		static void    Shutdown();
		static bool	   IsRunning();

		Core();
		~Core();
		void Init();
		void LateInit();
		void Setup();
		void Run();
	private:
		GameState              _game_state;
		SystemManager          _system_manager;
		unique_ptr<Scheduler>  _scheduler;
		unique_ptr<mt::ThreadPool> _thread_pool;
		atomic<bool>           _running;
		static inline Core*    _instance = nullptr;
		bool                   _setup = false;
		bool                   _shutdown = false;
	};
}
#include "Core.inl"