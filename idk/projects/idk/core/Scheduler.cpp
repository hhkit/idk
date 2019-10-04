#include "stdafx.h"
#include "Scheduler.h"

namespace idk
{
	Scheduler::Scheduler()
	{
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
		_accumulated_dt += std::min(_real_dt, dt_limit);

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

		execute_pass(_always_update);

		while (_accumulated_dt > _fixed_dt)
		{
			_accumulated_dt -= _fixed_dt;
			execute_pass(_fixed_update);
		}

		execute_pass(_prerender_update);
		execute_pass(_postrender_update);

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
	span<Scheduler::Pass> Scheduler::GetPasses(UpdatePhase phase)noexcept
	{
		switch (phase)
		{
		case UpdatePhase::Fixed:     return span<Pass>(_fixed_update);
		default:
		case UpdatePhase::Update:    return span<Pass>(_always_update);
		case UpdatePhase::PreRender: return span<Pass>(_prerender_update);
		case UpdatePhase::Render:    return span<Pass>(_postrender_update);
		};
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