#include "stdafx.h"
#include <core/IncludeSystems.h>
#include <core/IncludeComponents.h>

namespace idk
{
	Core::Core(std::shared_ptr<Application>&& app, std::shared_ptr<IEditor>&& editor)
		: _system_manager(std::move(app), std::move(editor))
	{
		_instance = this;
	}

	void Core::Run()
	{
		_system_manager.InitSystems();

		// main loop

		_system_manager.ShutdownSystems();
	}
}