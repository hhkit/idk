#include "stdafx.h"
#include "SystemManager.h"
#include <IncludeSystems.h>
#include <ds/ranged_for.h>

namespace idk
{
	SystemManager::SystemManager()
		: _list{Helper::InstantiateEngineSystems()}
	{
	}

	SystemManager::~SystemManager() = default;

	void SystemManager::InitSystems()
	{
		for (auto& elem : _list)
			if (elem)
				elem->Init();
	}
	void SystemManager::LateInitSystems()
	{
		for (auto& elem : _list)
			if (elem)
				elem->LateInit();
	}
	void SystemManager::EarlyShutdownSystems()
	{
		for (auto& elem : reverse(_list))
			if (elem)
				elem->EarlyShutdown();
	}
	void SystemManager::ShutdownSystems()
	{
		for (auto& elem : reverse(_list))
			if (elem)
				elem->Shutdown();
	}
}
