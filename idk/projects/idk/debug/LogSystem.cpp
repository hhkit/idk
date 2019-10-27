#include "stdafx.h"
#include "LogSystem.h"
#include <algorithm>
#include <iomanip>

#include <core/Scheduler.h>
#include <file/FileSystem.h>
namespace idk
{
	static auto curr_date()
	{
		using SystemClock = std::chrono::system_clock;
		const auto time = SystemClock::to_time_t(SystemClock::now());
		char buf[64];
		ctime_s(+buf, 64, &time);
		std::remove(std::begin(buf), std::end(buf), ' ');
		return string{ +buf };
	}

	LogSystem::~LogSystem()
	{
	}

	void LogSystem::Init()
	{
		auto logroot = string{ Core::GetSystem<FileSystem>().GetAppDataDir() } + "/idk/" + curr_date();
		auto start = Core::GetScheduler().GetProgramStart();
		constexpr array<string_view, s_cast<size_t>(LogPool::COUNT)> names
		{
			"main",
			"sys",
			"phys",
			"gfx",
			"game",
		};

		for (unsigned i = 0; i < s_cast<unsigned>(LogPool::COUNT); ++i)
		{
			auto& stream = log_files[i];
			stream.open(logroot + string{ names[i] } +".txt");
			stream << std::setw(2);

			LogSingleton::Get().SignalFor(s_cast<LogPool>(i)).Listen(
				[&stream, start](LogLevel level, time_point time, string_view preface, string_view message)
			{
				char buf[512];

				switch (level)
				{
				case LogLevel::INFO:    strcpy_s(buf, "[LOG  ]"); break;
				case LogLevel::WARNING: strcpy_s(buf, "[WARN ]"); break;
				case LogLevel::ERR:     strcpy_s(buf, "[ERROR]"); break;
				case LogLevel::FATAL:   strcpy_s(buf, "[FATAL]"); break;
				}
				int moved = 7;

				const auto time_since_start = time - start;

				int h = duration_cast<std::chrono::hours>(time_since_start).count();
				int m = duration_cast<std::chrono::minutes>(time_since_start).count() % 60;
				int s = duration_cast<std::chrono::seconds>(time_since_start).count() % 60;
				int ms = duration_cast<std::chrono::milliseconds>(time_since_start).count() % 100;

				moved += sprintf_s(buf + moved, sizeof(buf) - moved, "%d:%.2d:%.2d.%.3d:", h, m, s, ms);
				moved += sprintf_s(buf + moved, sizeof(buf) - moved, preface.data());
				sprintf_s(buf + moved, sizeof(buf) - moved, message.data());

				stream << buf << '\n';
			}
			);
		}
	}
}
