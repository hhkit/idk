#include "stdafx.h"
#include <core/Scheduler.h>
#include <IncludeSystems.h>
#include <IncludeComponents.h>
#include <res/ResourceManager.h>
#include <prefab/PrefabFactory.h>
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
        GetResourceManager().RegisterFactory<PrefabFactory>();
		_system_manager.LateInitSystems();
		_setup = true;
	}

	void Core::Run()
	{
		if (_setup == false)
			Setup();

		using mono::ScriptSystem;
		auto* editor = &GetSystem<IEditor>();

		// setup loop
		_scheduler->ScheduleFencedPass<UpdatePhase::FrameStart>(&ResourceManager::EmptyNewResources,   "Clear new resources");
		_scheduler->ScheduleFencedPass<UpdatePhase::FrameStart>(&ScriptSystem::ScriptStart,            "Start and Awake Scripts");

		_scheduler->ScheduleFencedPass<UpdatePhase::Fixed>     (&ScriptSystem::ScriptFixedUpdate,      "Script Fixed Update");
		_scheduler->SchedulePass      <UpdatePhase::Fixed>     (&TestSystem::TestSpan,                 "Test system until scripts are up");
		_scheduler->SchedulePass      <UpdatePhase::Fixed>     (&PhysicsSystem::PhysicsTick,           "Physics Update")
			                                      .IfPausedThen(&PhysicsSystem::DebugDrawColliders);
		_scheduler->ScheduleFencedPass<UpdatePhase::Fixed>     (&PhysicsSystem::FirePhysicsEvents,     "Trigger and Collision Events");
		
		_scheduler->SchedulePass      <UpdatePhase::MainUpdate>(&Application::PollEvents,              "Poll OS Events");
		_scheduler->SchedulePass      <UpdatePhase::MainUpdate>(&GamepadSystem::Update,                "Update gamepad states");
		_scheduler->SchedulePass      <UpdatePhase::MainUpdate>(&FileSystem::Update,                   "Check for file changes");
		_scheduler->SchedulePass      <UpdatePhase::MainUpdate>(&AudioSystem::Update,                  "Update listeners and sources");
		_scheduler->SchedulePass      <UpdatePhase::MainUpdate>(&UISystem::Update,                     "Update UI");
		_scheduler->ScheduleFencedPass<UpdatePhase::MainUpdate>(&ScriptSystem::ScriptUpdate,           "Update Scripts");
		_scheduler->ScheduleFencedPass<UpdatePhase::MainUpdate>(&ScriptSystem::ScriptUpdateCoroutines, "Update Coroutines");
		_scheduler->SchedulePass      <UpdatePhase::MainUpdate>(&AnimationSystem::Update,              "Animate animators")
												  .IfPausedThen(&AnimationSystem::UpdatePaused);
        _scheduler->SchedulePass      <UpdatePhase::MainUpdate>(&ParticleSystemUpdater::Update,        "Update Particle Systems")
												  .IfPausedThen(&ParticleSystemUpdater::EditorUpdate);
		_scheduler->ScheduleFencedPass<UpdatePhase::MainUpdate>(&ScriptSystem::ScriptLateUpdate,       "Late Update Scripts");
		_scheduler->ScheduleFencedPass<UpdatePhase::MainUpdate>(&SceneManager::DestroyQueuedObjects,         "Destroy Objects");
		_scheduler->ScheduleFencedPass<UpdatePhase::MainUpdate>(&SceneManager::BuildSceneGraph,        "Build scene graph");

		if (editor)
		{
		_scheduler->ScheduleFencedPass<UpdatePhase::MainUpdate>(&ResourceManager::WatchDirectory,      "Watch files");
		_scheduler->ScheduleFencedPass<UpdatePhase::MainUpdate>(&IEditor::EditorUpdate,                "Editor Update");
		_scheduler->ScheduleFencedPass<UpdatePhase::MainUpdate>(&SceneManager::DestroyQueuedObjects,         "Destroy Objects Again");
		_scheduler->ScheduleFencedPass<UpdatePhase::MainUpdate>(&SceneManager::BuildSceneGraph,        "Build scene graph");
		_scheduler->ScheduleFencedPass<UpdatePhase::MainUpdate>(&ResourceManager::SaveDirtyMetadata,   "Save dirty resources");
		_scheduler->ScheduleFencedPass<UpdatePhase::MainUpdate>(&ResourceManager::SaveDirtyFiles,      "Save dirty files");
		}

        _scheduler->SchedulePass      <UpdatePhase::PreRender> (&UISystem::FinalizeMatrices,           "Finalize UI Matrices");
		_scheduler->SchedulePass      <UpdatePhase::PreRender> (&GraphicsSystem::SortCameras,          "Sort Cameras");
		_scheduler->SchedulePass      <UpdatePhase::PreRender> (&GraphicsSystem::PrepareLights,        "Prepare Lights");
		_scheduler->SchedulePass      <UpdatePhase::PreRender> (&GraphicsSystem::BufferGraphicsState,  "Buffer graphics objects");

		_scheduler->SchedulePass      <UpdatePhase::Render>    (&GraphicsSystem::Prerender,            "Prerender");
		_scheduler->ScheduleFencedPass<UpdatePhase::Render>    (&GraphicsSystem::RenderRenderBuffer,   "Render Render Buffer");
		_scheduler->ScheduleFencedPass<UpdatePhase::Render>    (&DebugRenderer::GraphicsTick,          "Update durations of debug draw");
		_scheduler->ScheduleFencedPass<UpdatePhase::Render>    (&IEditor::EditorDraw,                  "Editor Draw");
		_scheduler->SchedulePass      <UpdatePhase::Render>    (&GraphicsSystem::SwapBuffer,           "Swap the buffers");
		// _scheduler->ScheduleFencedPass<UpdatePhase::MainUpdate>(&SceneManager::BuildSceneGraph,        "Build final scene graph");
	
		// main loop
		_scheduler->Setup();
		auto& app = Core::GetSystem<Application>();

        Core::GetSystem<SceneManager>().BuildSceneGraph(Core::GetGameState().GetObjectsOfType<const GameObject>());

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
	bool Core::IsRunning()
	{
		return _instance->_running;
	}
}