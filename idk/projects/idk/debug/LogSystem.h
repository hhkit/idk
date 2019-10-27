#pragma once
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
	private:
		void Init() override;
		void Shutdown() override {}

		array<std::fstream, s_cast<size_t>(LogPool::COUNT)> log_files;
	};
}