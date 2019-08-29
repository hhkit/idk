#include "stdafx.h"
#include <core/Scheduler.h>
#include <IncludeSystems.h>
#include <IncludeComponents.h>
#include <res/ResourceManager.h>

#include <gfx/GfxDbgTest.h>

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

	void Core::Setup()
	{
		_system_manager.InitSystems();
		_system_manager.LateInitSystems();
		_setup = true;
	}

	void Core::Run()
	{
		if (_setup == false)
			Setup();

		auto editor = &GetSystem<IEditor>();
		// setup loop
		_scheduler->ScheduleFencedPass<UpdatePhase::PostRender>(&Application::SwapBuffers, "Swap buffers");
		_scheduler->SchedulePass      <UpdatePhase::Update>    (&Application::PollEvents,  "Poll OS Events");
		_scheduler->SchedulePass      <UpdatePhase::Update>    (&FileSystem::Update, "File System Update");
		_scheduler->SchedulePass      <UpdatePhase::Update>    (&AudioSystem::Update,  "FMOD Update");
		_scheduler->SchedulePass      <UpdatePhase::Fixed>     (&TestSystem::TestSpan, "Test updates");
		_scheduler->ScheduleFencedPass<UpdatePhase::Update>    (&ResourceManager::WatchDirectory, "watch files");
		if (editor)
		{
			//_scheduler->SchedulePass      <UpdatePhase::PostRender>(&GraphicsSystem::BufferGraphicsState, "Buffer graphics objects");	
			//_scheduler->SchedulePass      <UpdatePhase::PostRender>(&GraphicsSystem::RenderBuffer, "Buffer graphics objects");
			_scheduler->ScheduleFencedPass<UpdatePhase::Update>(&IEditor::EditorUpdate, "Editor Update");

			_scheduler->SchedulePass      <UpdatePhase::PostRender>(&GraphicsSystem::BufferGraphicsState, "Buffer graphics objects");
			_scheduler->ScheduleFencedPass      <UpdatePhase::PostRender>(&GraphicsSystem::RenderRenderBuffer, "Buffer graphics objects");
			_scheduler->ScheduleFencedPass      <UpdatePhase::PostRender>(&IEditor::EditorDraw, "Editor Draw");
			_scheduler->SchedulePass      <UpdatePhase::PostRender>(&GraphicsSystem::SwapBuffer, "Buffer graphics objects");
		}
		else
		{
			_scheduler->SchedulePass      <UpdatePhase::PostRender>(&GraphicsSystem::BufferGraphicsState, "Buffer graphics objects");
			_scheduler->SchedulePass      <UpdatePhase::PostRender>(&GraphicsSystem::RenderRenderBuffer, "Buffer graphics objects");
			_scheduler->SchedulePass      <UpdatePhase::PostRender>(&GraphicsSystem::SwapBuffer, "Buffer graphics objects");
		}
		Core::GetSystem<FileSystem>();
		// main loop
		_scheduler->Setup();
		auto& app = Core::GetSystem<Application>();

		if (editor)
		{
			while (_running)
			{
				GfxDebugTest();
				_scheduler->SequentialUpdate();
				if (app.GetKey(Key::Alt) && app.GetKeyDown(Key::F4))
					Core::Shutdown();
			}
		}
		else
		{
			while (_running)
			{
				GfxDebugTest();
				_scheduler->SequentialUpdate(); // to swap for parallelized update in the future
				if (app.GetKey(Key::Alt) && app.GetKeyDown(Key::F4))
					Core::Shutdown();
				
			}
		}

		_system_manager.ShutdownSystems();
	}
	void Core::Shutdown()
	{
		_instance->_running = false;
	}
}