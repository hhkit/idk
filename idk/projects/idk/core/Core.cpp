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

	Core::~Core()
	{
		if (_setup && _shutdown == false)
			_system_manager.ShutdownSystems();
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
		_scheduler->SchedulePass      <UpdatePhase::Update>    (&Application::PollEvents,         "Poll OS Events");
		_scheduler->SchedulePass      <UpdatePhase::Update>    (&FileSystem::Update,              "File System Update");
		_scheduler->SchedulePass      <UpdatePhase::Update>    (&AudioSystem::Update,             "FMOD Update");
		_scheduler->SchedulePass      <UpdatePhase::Update>    (&AnimationSystem::Update,         "Animation Update");
		_scheduler->ScheduleFencedPass<UpdatePhase::Update>    (&SceneManager::DestroyObjects,    "Destroy Objects");
		_scheduler->ScheduleFencedPass<UpdatePhase::Update>    (&SceneManager::BuildSceneGraph,   "Build scene graph");
		_scheduler->SchedulePass      <UpdatePhase::Fixed>     (&TestSystem::TestSpan,            "Test updates");
		_scheduler->SchedulePass      <UpdatePhase::Fixed>     (&PhysicsSystem::ApplyGravity,     "Apply Gravity");
		_scheduler->SchedulePass      <UpdatePhase::Fixed>     (&PhysicsSystem::PredictTransform, "Predict Collisions");
		_scheduler->SchedulePass      <UpdatePhase::Fixed>     (&PhysicsSystem::CollideObjects,   "Collision Resolution");
		_scheduler->SchedulePass      <UpdatePhase::Fixed>     (&PhysicsSystem::MoveObjects,      "Move Objects");
		if (editor)
		{
			_scheduler->ScheduleFencedPass<UpdatePhase::Update>    (&ResourceManager::WatchDirectory,         "Watch files");
			_scheduler->ScheduleFencedPass<UpdatePhase::Update>    (&IEditor::EditorUpdate,                   "Editor Update");
			_scheduler->ScheduleFencedPass<UpdatePhase::Update>    (&ResourceManager::SaveDirtyMetadata,      "Save dirty resources");
			_scheduler->ScheduleFencedPass<UpdatePhase::Update>    (&SaveableResourceManager::SaveDirtyFiles, "Save dirty files");
			_scheduler->SchedulePass      <UpdatePhase::PostRender>(&GraphicsSystem::BufferGraphicsState,     "Buffer graphics objects");
			_scheduler->ScheduleFencedPass<UpdatePhase::PostRender>(&GraphicsSystem::RenderRenderBuffer,      "Render Render Buffer");
			_scheduler->ScheduleFencedPass<UpdatePhase::PostRender>(&DebugRenderer::GraphicsTick, "Update durations of debug draw");
			_scheduler->ScheduleFencedPass<UpdatePhase::PostRender>(&IEditor::EditorDraw,                     "Editor Draw");
			_scheduler->SchedulePass      <UpdatePhase::PostRender>(&GraphicsSystem::SwapBuffer,              "Swap the buffers");
		}
		else
		{
			_scheduler->ScheduleFencedPass<UpdatePhase::Update>(&SaveableResourceManager::SaveDirtyFiles, "Save dirty files");
			_scheduler->SchedulePass      <UpdatePhase::PostRender>(&GraphicsSystem::BufferGraphicsState, "Buffer graphics objects");
			_scheduler->SchedulePass      <UpdatePhase::PostRender>(&GraphicsSystem::RenderRenderBuffer,  "Render Render Buffer");
			_scheduler->ScheduleFencedPass<UpdatePhase::PostRender>(&DebugRenderer::GraphicsTick, "Update durations of debug draw");
			_scheduler->SchedulePass      <UpdatePhase::PostRender>(&GraphicsSystem::SwapBuffer,          "Swap the buffers");
		}
		Core::GetSystem<FileSystem>();
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

		_system_manager.ShutdownSystems();
		_shutdown = true;
	}
	void Core::Shutdown()
	{
		_instance->_running = false;
	}
}