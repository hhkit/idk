#pragma once
#include <idk.h>
#include <core/ISystem.h>
#include <core/SystemManager_detail.h>
#include <meta/meta.h>

namespace idk
{
	class SystemManager
	{
	public:
		SystemManager();
		~SystemManager();

		template<typename T>
		T& GetSystem();

		template<typename T, typename ...Args>
		T& AddSystem(Args&& ... args);

		void InitSystems();
		void LateInitSystems();
		void EarlyShutdownSystems();
		void ShutdownSystems();
	private:
		using SystemPtr = shared_ptr<ISystem>;
		using SystemList = array<SystemPtr, SystemCount>;

		SystemList _list;

		template<typename T>
		friend struct detail::SystemHelper;
		using Helper = detail::SystemHelper<Systems>;
	};
}

#include "SystemManager.inl"