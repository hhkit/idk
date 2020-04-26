#pragma once
#include <chrono>
namespace idk::vkn::dbg
{
	using milliseconds = std::chrono::duration<float, std::milli>;
	class stopwatch
	{
	public:
		using clock =std::chrono::high_resolution_clock;
		void start();
		//duration since the last lap() call or start() call, which ever is most recent.
		milliseconds lap();
		void stop();
		//the time between start and stop.
		milliseconds time()const;
	private:
		clock::time_point start_time{};
		clock::time_point end_time{};
		clock::time_point last_lap_time{};
	};

}