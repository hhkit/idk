#include "stdafx.h"
#include "Scheduler.inl"
#include <ds/span.inl>

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
		_accumulated_network_dt = _accumulated_fixed_dt = _accumulated_real_dt = seconds{};
	}
	void Scheduler::SequentialUpdate()
	{
		constexpr auto dt_limit = seconds{0.25};

		_this_frame = Clock::now();
		_real_dt = duration_cast<seconds>(_this_frame - _last_frame);
		_accumulated_real_dt += std::min(_real_dt, dt_limit);
		_accumulated_fixed_dt += std::min(_real_dt, dt_limit) * time_scale;

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


		while (_accumulated_real_dt > _game_update)
		{
			_accumulated_real_dt -= _game_update;
			_accumulated_network_dt += _game_update;

			const auto execute_network = _accumulated_network_dt > _network_update;
			if (execute_network)
				execute_pass(_passes[s_cast<size_t>(UpdatePhase::NetworkTickStart)]);

			execute_pass(_passes[s_cast<size_t>(UpdatePhase::FrameStart)]);

			while (_accumulated_fixed_dt > _fixed_dt)
			{
				_accumulated_fixed_dt -= _fixed_dt;
				execute_pass(_passes[s_cast<size_t>(UpdatePhase::Fixed)]);
			}

			execute_pass(_passes[s_cast<size_t>(UpdatePhase::MainUpdate)]);

			if (execute_network)
			{
				execute_pass(_passes[s_cast<size_t>(UpdatePhase::NetworkTickEnd)]);
				_accumulated_network_dt -= _network_update;
			}
		};

		execute_pass(_passes[s_cast<size_t>(UpdatePhase::PreRender)]);
		execute_pass(_passes[s_cast<size_t>(UpdatePhase::Render)]);

		_last_frame = _this_frame;
	}
	void Scheduler::ResetTimings()
	{
		_accumulated_fixed_dt = _fixed_dt;
		_accumulated_real_dt = _game_update;
	}
	seconds Scheduler::GetFixedDeltaTime()noexcept
	{
		return _fixed_dt;
	}
	seconds Scheduler::GetDeltaTime()noexcept
	{
		return _game_update * time_scale;
	}
	seconds Scheduler::GetUnscaledDeltaTime() noexcept
	{
		return _game_update;
	}
	seconds Scheduler::GetRemainingTime() noexcept
	{
		return _accumulated_real_dt;
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