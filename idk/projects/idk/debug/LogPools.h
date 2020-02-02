#pragma once

namespace idk
{
	enum class LogPool
	{
		ANY,
		SYS,
		PHYS,
		GFX,
		ANIM,
		MONO,
		GAME,
		EDIT,
		NETWORK,
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