#include "stdafx.h"
#include "SystemManager.h"
#include <core/SystemIncludes.h>
#include <util/ranged_for.h>

namespace idk
{
	SystemManager::SystemManager(shared_ptr<Application>&& application, shared_ptr<EditorSystem>&& editor)
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
