#include "stdafx.h"
#include "Scheduler.h"

namespace idk
{
	Scheduler::Scheduler()
	{
		_program_start = Clock::now();
		for (auto& elem : _systems_paused)
			elem = false;
	}
	void Scheduler::Setup()
	{

		_last_frame = _this_frame = Clock::now();
		_accumulated_dt = seconds{};
	}
	void Scheduler::SequentialUpdate()
	{
		constexpr auto dt_limit = seconds{0.25};

		_this_frame = Clock::now();
		_real_dt = duration_cast<seconds>(_this_frame - _last_frame);
		_accumulated_dt += std::min(_real_dt, dt_limit) * time_scale;

		constexpr auto execute_pass = [](auto& pass_vector)
		{
			for (auto& elem : pass_vector)
			{
				const time_point pt = Clock::now();
				bool paused = false;
				if (!elem.call() && elem.paused_call)
				{
					elem.paused_call();
					paused = true;
				}
				const time_point end = Clock::now();
				elem.previous_frames.push_back(Pass::Call{ duration_cast<seconds>(end - pt), paused });
			}
		};

		execute_pass(_passes[s_cast<size_t>(UpdatePhase::FrameStart)]);

		while (_accumulated_dt > _fixed_dt)
		{
			_accumulated_dt -= _fixed_dt;
			execute_pass(_passes[s_cast<size_t>(UpdatePhase::Fixed)]);
		}

		execute_pass(_passes[s_cast<size_t>(UpdatePhase::MainUpdate)]);


		execute_pass(_passes[s_cast<size_t>(UpdatePhase::PreRender)]);
		execute_pass(_passes[s_cast<size_t>(UpdatePhase::Render)]);

		_last_frame = _this_frame;
	}
	seconds Scheduler::GetDeltaTime()noexcept
	{
		return _fixed_dt;
	}
	seconds Scheduler::GetRealDeltaTime()noexcept
	{
		return _real_dt;
	}
	time_point Scheduler::GetProgramStart() noexcept
	{
		return _program_start;
	}
	span<Scheduler::Pass> Scheduler::GetPasses(UpdatePhase phase)noexcept
	{
		return span<Pass>(_passes[s_cast<size_t>(phase)]);
	}
	Scheduler::Pass::Pass(Lock read, Lock write, FnPtr call, string_view update_name)
		: read_components{ read }, write_components{ write }, call{ call }, update_name{update_name}
	{
	}
	string_view Scheduler::Pass::Name() const
	{
		return update_name;
	}
	const Scheduler::Pass::PerformanceGraph& Scheduler::Pass::Graph() const
	{
		return previous_frames;
	}
}