#pragma once
#include <fstream>

#include <idk.h>
#include <core/ISystem.h>
#include <debug/LogSingleton.h>
#include <filesystem>

namespace idk
{
	class LogSystem
		: public ISystem
	{
	public:
		~LogSystem();
		static constexpr size_t log_buffer_size = 2048;
	private:
		struct LogHandle
		{
			string filepath;
			std::ofstream stream;
			unsigned int signal_id;
		};

		void Init() override;
		void Shutdown() override {}
		array<LogHandle, s_cast<size_t>(LogPool::COUNT)> log_files;
	};
}