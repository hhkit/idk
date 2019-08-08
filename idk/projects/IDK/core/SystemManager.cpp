#include "stdafx.h"
#include "SystemManager.h"
#include <core/IncludeSystems.h>
#include <ds/ranged_for.h>

namespace idk
{
	SystemManager::SystemManager(shared_ptr<Application>&& application, shared_ptr<IEditor>&& editor)
		: _list{Helper::InstantiateEngineSystems(std::move(application), std::move(editor))}
	{
	}

	SystemManager::~SystemManager() = default;

	void SystemManager::InitSystems()
	{
		for (auto& elem : _list)
			if (elem)
				elem->Init();
	}
	void SystemManager::ShutdownSystems()
	{
		for (auto& elem : reverse(_list))
			if (elem)
				elem->Shutdown();
	}
}
