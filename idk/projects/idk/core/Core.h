#pragma once
#include <core/SystemManager.h>

namespace idk
{
	class GameState;
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
		static GameState& GetGameState();
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
		struct PImpl;

		SystemManager& GetSystemManager();
		const SystemManager& GetSystemManager()const;

		static inline Core* _instance = nullptr;
		std::unique_ptr<PImpl> _pimpl;
	};
}
#include "Core.inl"