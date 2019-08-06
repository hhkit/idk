#include "stdafx.h"
#include <core/Scheduler.h>
#include <core/IncludeSystems.h>
#include <core/IncludeComponents.h>

namespace idk
{
	seconds Core::GetDT()
	{
		return _instance->_scheduler->GetDeltaTime();
	}
	seconds Core::GetRealDT()
	{
		return _instance->_scheduler->GetRealDeltaTime();
	}
	Core::Core(std::shared_ptr<Application>&& app, std::shared_ptr<IEditor>&& editor)
		: _system_manager(std::move(app), std::move(editor)), _scheduler(std::make_unique<Scheduler>()), _running{true}
	{
		_instance = this;
	}

	Core::~Core() = default;

	void Core::Run()
	{
		_system_manager.InitSystems();

		// setup loop
		_scheduler->SchedulePass      <UpdatePhase::Update>   (&Application::PollEvents,  "Poll OS Events");
		_scheduler->SchedulePass      <UpdatePhase::Fixed>    (&TestSystem::TestSpan,     "Test updates");
		_scheduler->ScheduleFencedPass<UpdatePhase::PreRender>(&Application::SwapBuffers, "Swap buffers");

		// main loop
		_scheduler->Setup();
		if (&GetSystem<IEditor>())
		{
			while (_running)
				_scheduler->SequentialUpdate();
		}
		else
		{
			while (_running)
				_scheduler->SequentialUpdate();
		}

		_system_manager.ShutdownSystems();
	}
	void Core::Shutdown()
	{
		_instance->_running = false;
	}
}