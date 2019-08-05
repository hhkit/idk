#pragma once
#include <idk.h>
#include <core/Systems.h>
#include <core/ISystem.h>
#include <util/meta.h>

namespace idk
{
	class SystemManager
	{
	public:
		SystemManager(shared_ptr<Application>&& application, shared_ptr<IEditor>&& ptr);
		~SystemManager();

		template<typename T>
		T& GetSystem();

		void InitSystems();
		void ShutdownSystems();
	private:
		using SystemPtr = shared_ptr<ISystem>;
		using SystemList = array<SystemPtr, SystemCount>;

		SystemList _list;

		template<typename T>
		friend struct detail::SystemHelper;
		using Helper = detail::SystemHelper<EngineSystems>;
	};
}

#include "SystemManager.inl"