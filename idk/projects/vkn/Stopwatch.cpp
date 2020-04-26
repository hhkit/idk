#include "pch.h"
#include "Stopwatch.h"

namespace idk::vkn::dbg
{

void stopwatch::start()
{
	last_lap_time = start_time = clock::now();
}

milliseconds stopwatch::lap()
{
	auto prev_lap_time = last_lap_time;
	last_lap_time = clock::now();
	return std::chrono::duration_cast<milliseconds>(last_lap_time-prev_lap_time);
}

void stopwatch::stop()
{
	end_time = clock::now();
}

milliseconds stopwatch::time()const
{
	return std::chrono::duration_cast<milliseconds>(end_time-start_time);
}

}