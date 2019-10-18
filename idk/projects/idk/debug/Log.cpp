#include "stdafx.h"
#include <idk.h>
#include "Log.h"
#include "LogSystem.h"

namespace idk
{
	void Log(LogPool pool, std::string_view preface, std::string_view message)
	{
		Core::GetSystem<LogSystem>().LogMessage(pool, string{ preface } +string{ message });
	}
}
