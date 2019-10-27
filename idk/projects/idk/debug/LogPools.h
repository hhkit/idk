#pragma once

namespace idk
{
	enum class LogPool
	{
		ANY,
		SYS,
		PHYS,
		GFX,
		GAME,
		COUNT
	};

	enum class LogLevel
	{
		INFO,
		WARNING,
		ERR,
		FATAL,
	};
}