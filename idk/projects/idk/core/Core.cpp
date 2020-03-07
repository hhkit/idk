#include "stdafx.h"
#include <core/GameState.inl>
#include <core/Scheduler.inl>
#include <IncludeSystems.h>
#include <IncludeComponents.h>
#include <res/ResourceHandle.inl>
#include <res/ResourceManager.inl>
#include <res/ResourceHandle.inl>
#include <prefab/PrefabFactory.h>
#include <gfx/GfxDbgTest.h>
#include <parallel/ThreadPool.h>
#include <ds/span.inl>

namespace idk
{
	struct Core::PImpl
	{
		GameState              _game_state;
		SystemManager          _system_manager;
		unique_ptr<Scheduler>  _scheduler;
		unique_ptr<mt::ThreadPool> _thread_pool;
		atomic<bool>           _running;
		bool                   _setup = false;
		bool                   _shutdown = false;
		PImpl():_system_manager(), _scheduler(std::make_unique<Scheduler>()), _running{ true }
		{}
		~PImpl()
		{
			if (_setup && _shutdown == false)
			{
				_system_manager.EarlyShutdownSystems();
				_system_manager.ShutdownSystems();
			}
		}
	};
	seconds Core::GetDT()
	{
		return _instance->_pimpl->_scheduler->GetFixedDeltaTime();
	}
	seconds Core::GetRealDT()
	{
		return _instance->_pimpl->_scheduler->GetDeltaTime();
	}

	GameState& Core::GetGameState() { return _instance->_pimpl->_game_state; }

	Scheduler& Core::GetScheduler()
	{
		return *_instance->_pimpl->_scheduler;
	}

	mt::ThreadPool& Core::GetThreadPool()
	{
		return *_instance->_pimpl->_thread_pool;
	}
	
	ResourceManager& Core::GetResourceManager()
	{
		return GetSystem<ResourceManager>();
	}

	Core::Core()
		: _pimpl{std::make_unique<PImpl>()}
	{
		_instance = this;
	}

	Core::~Core()
	{
	}

	void Core::Init()
	{
		_pimpl->_system_manager.InitSystems();
	}

	void Core::LateInit()
	{
		GetResourceManager().RegisterFactory<PrefabFactory>();
		_pimpl->_system_manager.LateInitSystems();
		_pimpl->_setup = true;
	}

	void Core::Setup()
	{
		Init();
		LateInit();
	}

	void Core::Run()
	{
		if (_pimpl->_setup == false)
			Setup();

		using mono::ScriptSystem;
		auto* editor = &GetSystem<IEditor>();

		// setup loop
		_pimpl->_scheduler->ScheduleFencedPass<UpdatePhase::FrameStart>(&SceneManager::ChangeScene,            "Change Scene");
		_pimpl->_scheduler->ScheduleFencedPass<UpdatePhase::FrameStart>(&ResourceManager::EmptyNewResources,   "Clear new resources");
		_pimpl->_scheduler->ScheduleFencedPass<UpdatePhase::NetworkTickStart>(&NetworkSystem::ReceivePackets,        "Receive Packets");
		_pimpl->_scheduler->ScheduleFencedPass<UpdatePhase::NetworkTickStart>(&NetworkSystem::RespondToPackets,      "Respond to Packets");
		_pimpl->_scheduler->ScheduleFencedPass<UpdatePhase::FrameStart>(&ScriptSystem::ScriptStart,            "Start and Awake Scripts");

		_pimpl->_scheduler->ScheduleFencedPass<UpdatePhase::Fixed>     (&ScriptSystem::ScriptFixedUpdate,      "Script Fixed Update");
		_pimpl->_scheduler->SchedulePass      <UpdatePhase::Fixed>     (&PhysicsSystem::PhysicsTick,           "Physics Update")
			                                              .IfPausedThen(&PhysicsSystem::DebugDrawColliders);
		_pimpl->_scheduler->ScheduleFencedPass<UpdatePhase::Fixed>     (&PhysicsSystem::FirePhysicsEvents,     "Trigger and Collision Events");

		_pimpl->_scheduler->SchedulePass      <UpdatePhase::MainUpdate>(&TestSystem::TestSpan,                 "Test system");
		_pimpl->_scheduler->SchedulePass      <UpdatePhase::MainUpdate>(&Application::PollEvents,              "Poll OS Events");
		_pimpl->_scheduler->SchedulePass      <UpdatePhase::MainUpdate>(&GamepadSystem::Update,                "Update gamepad states");
		_pimpl->_scheduler->SchedulePass      <UpdatePhase::MainUpdate>(&FileSystem::Update,                   "Check for file changes");
		_pimpl->_scheduler->SchedulePass      <UpdatePhase::MainUpdate>(&AudioSystem::Update,                  "Update listeners and sources");
		_pimpl->_scheduler->SchedulePass      <UpdatePhase::MainUpdate>(&UISystem::Update,                     "Update UI");
		_pimpl->_scheduler->ScheduleFencedPass<UpdatePhase::MainUpdate>(&ScriptSystem::ScriptUpdate,           "Update Scripts")
			                                              .IfPausedThen(&ScriptSystem::ScriptPausedUpdate);
		_pimpl->_scheduler->ScheduleFencedPass<UpdatePhase::MainUpdate>(&ScriptSystem::ScriptUpdateCoroutines, "Update Coroutines");
		_pimpl->_scheduler->SchedulePass      <UpdatePhase::MainUpdate>(&AnimationSystem::Update,              "Animate animators")
												          .IfPausedThen(&AnimationSystem::UpdatePaused);
        _pimpl->_scheduler->SchedulePass      <UpdatePhase::MainUpdate>(&ParticleSystemUpdater::Update,        "Update Particle Systems");
		_pimpl->_scheduler->ScheduleFencedPass<UpdatePhase::MainUpdate>(&ScriptSystem::ScriptLateUpdate,       "Late Update Scripts");
		_pimpl->_scheduler->ScheduleFencedPass<UpdatePhase::MainUpdate>(&SceneManager::DestroyQueuedObjects,   "Destroy Objects");
		_pimpl->_scheduler->ScheduleFencedPass<UpdatePhase::MainUpdate>(&SceneManager::BuildSceneGraph,        "Build scene graph");
		_pimpl->_scheduler->ScheduleFencedPass<UpdatePhase::NetworkTickEnd>(&NetworkSystem::PreparePackets,        "Prepare to Packets");
		_pimpl->_scheduler->ScheduleFencedPass<UpdatePhase::NetworkTickEnd>(&NetworkSystem::SendPackets,           "Send Packets");

		if (editor)
		{
		_pimpl->_scheduler->ScheduleFencedPass<UpdatePhase::PreRender>(&ResourceManager::WatchDirectory,      "Watch files");
		_pimpl->_scheduler->ScheduleFencedPass<UpdatePhase::PreRender>(&IEditor::EditorUpdate,                "Editor Update");
		_pimpl->_scheduler->ScheduleFencedPass<UpdatePhase::PreRender>(&SceneManager::DestroyQueuedObjects,         "Destroy Objects Again");
		_pimpl->_scheduler->ScheduleFencedPass<UpdatePhase::PreRender>(&SceneManager::BuildSceneGraph,        "Build scene graph");
		_pimpl->_scheduler->ScheduleFencedPass<UpdatePhase::PreRender>(&ResourceManager::SaveDirtyMetadata,   "Save dirty resources");
		_pimpl->_scheduler->ScheduleFencedPass<UpdatePhase::PreRender>(&ResourceManager::SaveDirtyFiles,      "Save dirty files");
		}


        _pimpl->_scheduler->SchedulePass      <UpdatePhase::PreRender> (&UISystem::FinalizeMatrices,           "Finalize UI Matrices");
		_pimpl->_scheduler->SchedulePass      <UpdatePhase::PreRender> (&GraphicsSystem::SortCameras,          "Sort Cameras");
		_pimpl->_scheduler->SchedulePass      <UpdatePhase::PreRender> (&GraphicsSystem::PrepareLights,        "Prepare Lights");
		_pimpl->_scheduler->SchedulePass      <UpdatePhase::PreRender> (&GraphicsSystem::BufferGraphicsState,  "Buffer graphics objects");

		_pimpl->_scheduler->SchedulePass      <UpdatePhase::Render>    (&GraphicsSystem::Prerender,            "Prerender");
		_pimpl->_scheduler->ScheduleFencedPass<UpdatePhase::Render>    (&GraphicsSystem::RenderRenderBuffer,   "Render Render Buffer");
		_pimpl->_scheduler->ScheduleFencedPass<UpdatePhase::Render>    (&DebugRenderer::GraphicsTick,          "Update durations of debug draw");
		_pimpl->_scheduler->ScheduleFencedPass<UpdatePhase::Render>    (&IEditor::EditorDraw,                  "Editor Draw");
		_pimpl->_scheduler->SchedulePass      <UpdatePhase::Render>    (&GraphicsSystem::SwapBuffer,           "Swap the buffers");
		// _pimpl->_scheduler->ScheduleFencedPass<UpdatePhase::MainUpdate>(&SceneManager::BuildSceneGraph,        "Build final scene graph");
	
		// main loop
		_pimpl->_scheduler->Setup();
		_pimpl->_thread_pool = std::make_unique<mt::ThreadPool>(std::thread::hardware_concurrency() - 2);
		auto& app = Core::GetSystem<Application>();

        Core::GetSystem<SceneManager>().BuildSceneGraph(Core::GetGameState().GetObjectsOfType<const GameObject>());

		if (editor)
		{
			while (_pimpl->_running)
			{
				_pimpl->_scheduler->SequentialUpdate();
				if (app.GetKey(Key::Alt) && app.GetKeyDown(Key::F4))
					Core::Shutdown();
			}
		}
		else
		{
			while (_pimpl->_running)
			{
				_pimpl->_scheduler->SequentialUpdate(); // to swap for parallelized update in the future
				if (app.GetKey(Key::Alt) && app.GetKeyDown(Key::F4))
					Core::Shutdown();
				
			}
		}
		_pimpl->_system_manager.EarlyShutdownSystems();
		_pimpl->_system_manager.ShutdownSystems();
		_pimpl->_shutdown = true;
	}
	SystemManager& Core::GetSystemManager()
	{
		return _pimpl->_system_manager;
	}
	const SystemManager& Core::GetSystemManager() const
	{
		return _pimpl->_system_manager;
	}
	void Core::Shutdown()
	{
		_instance->_pimpl->_running = false;
	}
	bool Core::IsRunning()
	{
		return _instance->_pimpl->_running;
	}
}