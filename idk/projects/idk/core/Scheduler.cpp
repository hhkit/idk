#include "stdafx.h"
#include "Scheduler.h"

namespace idk
{
	Scheduler::Scheduler()
	{
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

		for (auto& elem : _always_update)
			elem.call();

		while (_accumulated_dt > _fixed_dt)
		{
			_accumulated_dt -= _fixed_dt;
			for (auto& elem : _fixed_update)
				elem.call();
		}

		for (auto& elem : _prerender_update)
			elem.call();

		for (auto& elem : _postrender_update)
			elem.call();

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
}