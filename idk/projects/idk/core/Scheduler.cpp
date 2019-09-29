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

		auto execute_pass = [](const auto& pass_vector)
		{
			for (auto& elem : pass_vector)
				if (!elem.call() && elem.paused_call)
					elem.paused_call();
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
	seconds Scheduler::GetDeltaTime()
	{
		return _fixed_dt;
	}
	seconds Scheduler::GetRealDeltaTime()
	{
		return _real_dt;
	}
	Scheduler::Pass::Pass(Lock read, Lock write, FnPtr call, string_view update_name)
		: read_components{ read }, write_components{ write }, call{ call }, update_name{update_name}
	{
	}
}