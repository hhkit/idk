#include "stdafx.h"
#include "LogSystem.h"
#include <algorithm>
#include <iomanip>

#include <core/Scheduler.h>
#include <file/FileSystem.h>
#include <serialize/text.h>
namespace idk
{
	static auto curr_datetime()
	{
		using SystemClock = std::chrono::system_clock;
		const auto time = SystemClock::to_time_t(SystemClock::now());
		char buf[64];
		struct tm datetime{};
		localtime_s(&datetime, &time);
		sprintf_s(buf, "%.4d%.2d%.2d_%.2d%.2d%.2d", 1900 + datetime.tm_year, 1 + datetime.tm_mon, datetime.tm_mday, datetime.tm_hour, datetime.tm_min, datetime.tm_sec);
		return string{ +buf };
	}

	LogSystem::~LogSystem()
	{
		if (!Core::IsRunning())
		{
			int i = 0;
			for (auto& handle : log_files)
			{
				handle.stream.close();
				std::remove(handle.filepath.data());
				LogSingleton::Get().SignalFor(s_cast<LogPool>(i++)) -= handle.signal_id;
			}
		}
	}

    void LogSystem::SetLogDir(string_view dir)
    {
        _log_dir = dir;
    }
	thread_local string log_buffer(LogSystem::log_buffer_size*2,' ');
	void LogSystem::Init()
	{
        if (_log_dir.empty())
            throw; // set a log path first you dumbass

        const auto logroot = _log_dir + '/' + curr_datetime();
        const auto start = Core::GetScheduler().GetProgramStart();
		constexpr array<string_view, s_cast<size_t>(LogPool::COUNT)> names
		{
			"main",
			"sys",
			"phys",
			"gfx",
			"anim",
			"mono",
			"game",
			"edit",
		};

		for (unsigned i = 0; i < s_cast<unsigned>(LogPool::COUNT); ++i)
		{
			auto& loghandle = log_files[i];
			auto& stream = loghandle.stream;
			loghandle.filepath = logroot + "_" + serialize_text(i) + "_" + string{ names[i] } + ".txt";
			stream.open(loghandle.filepath);
			if (stream)
			loghandle.signal_id = LogSingleton::Get().SignalFor(s_cast<LogPool>(i)).Listen(
				[&stream, start](LogLevel level, time_point time, string_view preface, string_view message)
			{
				//char buf[log_buffer_size*2];
				if (log_buffer.size() < message.size())
					log_buffer.resize(message.size() * 2);
				auto buf = log_buffer.data();
				unsigned moved = 0;

				switch (level)
				{
				case LogLevel::INFO:    strcpy_s(buf,std::size(log_buffer), "[INFO]  "); break;
				case LogLevel::WARNING: strcpy_s(buf,std::size(log_buffer), "[WARN]  "); break;
				case LogLevel::ERR:     strcpy_s(buf,std::size(log_buffer), "[ERROR] "); break;
				case LogLevel::FATAL:   strcpy_s(buf,std::size(log_buffer), "[FATAL] "); break;
				}
				moved = 8;

				const auto time_since_start = time - start;

				int h = duration_cast<std::chrono::hours>(time_since_start).count();
				int m = duration_cast<std::chrono::minutes>(time_since_start).count() % 60;
				int s = duration_cast<std::chrono::seconds>(time_since_start).count() % 60;
				int ms = duration_cast<std::chrono::milliseconds>(time_since_start).count() % 100;

				moved += sprintf_s(buf + moved, std::size(log_buffer) - moved, "%d:%.2d:%.2d.%.3d: ", h, m, s, ms);
				moved += sprintf_s(buf + moved, std::size(log_buffer) - moved, preface.data());
				snprintf(buf + moved, sizeof(buf) - moved, "\t%s\n", message.data());
				stream << buf;
				if (level == LogLevel::FATAL)
					stream << std::flush;
			}
			);
		}
	}
}
