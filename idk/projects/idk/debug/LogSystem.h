#pragma once
#include <fstream>

#include <idk.h>
#include <core/ISystem.h>
#include <debug/LogSingleton.h>
#include <filesystem>

namespace idk
{
	namespace hack
	{
		struct LogSystemConfig
		{
			bool enabled = false;
			static LogSystemConfig& GetSingleton();
		};
	}
	class LogSystem
		: public ISystem
	{
	public:
		~LogSystem();
		static constexpr size_t log_buffer_size = 2048;
        void SetLogDir(string_view dir);

	private:
		struct LogHandle
		{
			string filepath;
			std::ofstream stream;
			SignalBase::SlotId signal_id;
		};

		void Init() override;
		void Shutdown() override {}
		array<LogHandle, static_cast<size_t>(LogPool::COUNT)> log_files;
        string _log_dir;
		bool _enabled = false;
	};
}