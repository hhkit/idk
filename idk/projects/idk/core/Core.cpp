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

	void Core::Setup()
	{
		_system_manager.InitSystems();
		_setup = true;
	}

	void Core::Run()
	{
		if (_setup == false)
			Setup();

		auto editor = &GetSystem<IEditor>();
		// setup loop
		_scheduler->SchedulePass      <UpdatePhase::Update>           (&Application::PollEvents , "Poll OS Events"    );
		_scheduler->SchedulePass      <UpdatePhase::Update>           (&FileSystem::Update      , "File System Update");
		_scheduler->SchedulePass      <UpdatePhase::Update>           (&AudioSystem::Update     , "FMOD Update"       );
		_scheduler->SchedulePass      <UpdatePhase::Fixed>            (&TestSystem::TestSpan    , "Test updates"      );
		if (editor) _scheduler->ScheduleFencedPass<UpdatePhase::Fixed>(&IEditor::EditorUpdate   , "Editor Update"     );
		_scheduler->ScheduleFencedPass<UpdatePhase::PostRender>       (&Application::SwapBuffers, "Swap buffers"      );

		Core::GetSystem<FileSystem>();
		// main loop
		_scheduler->Setup();
		if (editor)
		{
			while (_running)
			{
				_scheduler->SequentialUpdate();
				Core::GetSystem<GraphicsSystem>().RenderBuffer();
			}
		}
		else
		{
			while (_running)
			{
				_scheduler->SequentialUpdate(); // to swap for parallelized update in the future
				auto &dbg_renderer = Core::GetSystem<DebugRenderer>();
				if (&dbg_renderer)
				{
					int max_count = 10;
					for (int i = 0; i < max_count; ++i)
					{
						float chunk = 2.0f / max_count;
						dbg_renderer.DrawShape(DbgShape::eSquare, vec3{ -1.0f+chunk*i,0,0 }, vec3{ chunk,chunk,chunk}, vec3{ 0,0,1 }, rad{ pi *chunk*i }, vec4{ 0,1,0,1 });

					}
					//dbg_renderer.DrawShape(DbgShape::eSquare, vec3{ 0.2f,0.4f,0 }, vec3{ 0.2f,0.2f,1 }, vec3{ 0,0,1 }, rad{ pi / 6 }, vec4{ 0,1,0,1 });
					//dbg_renderer.DrawShape(DbgShape::eSquare, vec3{ 0.1f,0,0 }, vec3{ 1,1,1 }, vec3{ 0,0,1 }, rad{ pi / 2 }, vec4{ 0,1,0,1 });
					//dbg_renderer.DrawShape(DbgShape::eSquare, vec3{ 0.3f,-0.3f,0 }, vec3{ 1,1,1 }, vec3{ 0,0,1 }, rad{ pi / 5 }, vec4{ 0,1,0,1 });
					//dbg_renderer.DrawShape(DbgShape::eSquare, vec3{ -0.3f,0.2f,0 }, vec3{ 1,1,1 }, vec3{ 0,0,1 }, rad{ pi / 4 }, vec4{ 0,1,0,1 });
					dbg_renderer.DrawShape(DbgShape::eSquare, vec3{ -0.3f,0,0 }, vec3{ 1.0f,0.5f,1.0f }, vec3{ 0,0,1 }, rad{ pi/3 }, vec4{ 0,1,0,1 });
					dbg_renderer.Render();
				}
				//Core::GetSystem<GraphicsSystem>().EndFrame();
				Core::GetSystem<GraphicsSystem>().RenderBuffer();
			}
		}

		_system_manager.ShutdownSystems();
	}
	void Core::Shutdown()
	{
		_instance->_running = false;
	}
}