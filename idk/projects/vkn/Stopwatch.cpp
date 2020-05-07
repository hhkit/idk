#include "pch.h"
#include "Stopwatch.h"

namespace idk::vkn::dbg
{
	static inline constexpr bool active = false;

void stopwatch::start()
{
	if constexpr (active)
		last_lap_time = start_time = clock::now();
}

milliseconds stopwatch::lap()
{
	if constexpr(!active)
		return milliseconds{ 0 };
	else
	{
		auto prev_lap_time = last_lap_time;
		last_lap_time = clock::now();
		return std::chrono::duration_cast<milliseconds>(last_lap_time - prev_lap_time);
	}
}

void stopwatch::stop()
{
	if constexpr (active)
		end_time = clock::now();
}

milliseconds stopwatch::time()const
{
	if constexpr (!active)
		return milliseconds{ 0 };
	else
		return std::chrono::duration_cast<milliseconds>(end_time-start_time);
}

}