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

	Scheduler& Core::GetScheduler()
	{
		return *_instance->_scheduler;
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

	Core::~Core()
	{
		if (_setup && _shutdown == false)
		{
			_system_manager.EarlyShutdownSystems();
			_system_manager.ShutdownSystems();
		}
	}

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
		_scheduler->SchedulePass      <UpdatePhase::Update>(&Application::PollEvents,            "Poll OS Events");
		_scheduler->SchedulePass      <UpdatePhase::Update>(&FileSystem::Update,                 "Check for file changes");
		_scheduler->SchedulePass      <UpdatePhase::Update>(&AudioSystem::Update,                "Update listeners and sources");
		_scheduler->SchedulePass      <UpdatePhase::Update>(&AnimationSystem::Update,            "Animate animators");
		_scheduler->ScheduleFencedPass<UpdatePhase::Update>(&SceneManager::BuildSceneGraph,      "Build scene graph");
		_scheduler->ScheduleFencedPass<UpdatePhase::Update>(&SceneManager::DestroyObjects,       "Destroy Objects");
		_scheduler->SchedulePass      <UpdatePhase::Fixed> (&TestSystem::TestSpan,               "Test system until scripts are up");
		_scheduler->SchedulePass      <UpdatePhase::Fixed> (&PhysicsSystem::PhysicsTick,         "Physics Update")
			.IfPausedThen(&PhysicsSystem::DebugDrawColliders);

		if (editor)
		{
		_scheduler->ScheduleFencedPass<UpdatePhase::Update>(&ResourceManager::WatchDirectory,    "Watch files");
		_scheduler->ScheduleFencedPass<UpdatePhase::Update>(&IEditor::EditorUpdate,              "Editor Update");
		_scheduler->ScheduleFencedPass<UpdatePhase::Update>(&SceneManager::DestroyObjects,       "Destroy Objects Again");
		_scheduler->ScheduleFencedPass<UpdatePhase::Update>(&ResourceManager::SaveDirtyMetadata, "Save dirty resources");
		_scheduler->ScheduleFencedPass<UpdatePhase::Update>(&ResourceManager::SaveDirtyFiles,    "Save dirty files");
		}

		_scheduler->SchedulePass      <UpdatePhase::Update>(&GraphicsSystem::BufferGraphicsState,"Buffer graphics objects");

		_scheduler->SchedulePass      <UpdatePhase::Render>(&GraphicsSystem::Prerender,          "Prerender");
		_scheduler->ScheduleFencedPass<UpdatePhase::Render>(&GraphicsSystem::RenderRenderBuffer, "Render Render Buffer");
		_scheduler->ScheduleFencedPass<UpdatePhase::Render>(&DebugRenderer::GraphicsTick,        "Update durations of debug draw");
		_scheduler->ScheduleFencedPass<UpdatePhase::Render>(&IEditor::EditorDraw,                "Editor Draw");
		_scheduler->SchedulePass      <UpdatePhase::Render>(&GraphicsSystem::SwapBuffer,         "Swap the buffers");
	
		// main loop
		_scheduler->Setup();
		auto& app = Core::GetSystem<Application>();

		if (editor)
		{
			while (_running)
			{
				_scheduler->SequentialUpdate();
				if (app.GetKey(Key::Alt) && app.GetKeyDown(Key::F4))
					Core::Shutdown();
			}
		}
		else
		{
			while (_running)
			{
				_scheduler->SequentialUpdate(); // to swap for parallelized update in the future
				if (app.GetKey(Key::Alt) && app.GetKeyDown(Key::F4))
					Core::Shutdown();
				
			}
		}
		_system_manager.EarlyShutdownSystems();
		_system_manager.ShutdownSystems();
		_shutdown = true;
	}
	void Core::Shutdown()
	{
		_instance->_running = false;
	}
}