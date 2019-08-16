#include "stdafx.h"
#include <core/Scheduler.h>
#include <core/IncludeSystems.h>
#include <core/IncludeComponents.h>
#include <res/ResourceManager.h>

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
	
	ResourceManager& Core::GetResourceManager()
	{
		return GetSystem<ResourceManager>();
	}

	Core::Core()
		: _system_manager(), _scheduler(std::make_unique<Scheduler>()), _running{true}
	{
		_instance = this;
	}

	Core::~Core() = default;

	void Core::Run()
	{
		_system_manager.InitSystems();
		auto editor = &GetSystem<IEditor>();
		// setup loop
		_scheduler->SchedulePass      <UpdatePhase::Update>    (&Application::PollEvents,  "Poll OS Events");
		_scheduler->SchedulePass      <UpdatePhase::Update>(&FileSystem::Update, "File System Update");
		_scheduler->SchedulePass      <UpdatePhase::Update>    (&AudioSystem::Run,  "FMOD Update");
		_scheduler->SchedulePass      <UpdatePhase::Fixed>(&TestSystem::TestSpan, "Test updates");
		if (editor) _scheduler->ScheduleFencedPass<UpdatePhase::Fixed>(&IEditor::EditorUpdate, "Editor Update");
		_scheduler->ScheduleFencedPass<UpdatePhase::PostRender>(&Application::SwapBuffers, "Swap buffers");

		Core::GetSystem<FileSystem>();
		// main loop
		_scheduler->Setup();
		if (editor)
		{
			while (_running)
			{
				_scheduler->SequentialUpdate();
				Core::GetSystem<GraphicsSystem>().Draw();
			}
		}
		else
		{
			while (_running)
			{
				_scheduler->SequentialUpdate(); // to swap for parallelized update in the future
				Core::GetSystem<GraphicsSystem>().Draw();
			}
		}

		_system_manager.ShutdownSystems();
	}
	void Core::Shutdown()
	{
		_instance->_running = false;
	}
}