//////////////////////////////////////////////////////////////////////////////////
//@file		ConsoleLog.cpp
//@author	Muhammad Izha B Rahim
//@param	Email : izha95\@hotmail.com
//@date		20 OCT 2019
//@brief	

/*
Wrapper for printing into IGE_Console.
*/
//////////////////////////////////////////////////////////////////////////////////




#include "pch.h"
#include <widgets/ConsoleLog.h>
#include <editor/windows/IGE_Console.h>
#include <IDE.h>

namespace idk::console
{

	void Log_Message(std::string_view message, const vec4& textColor)
	{
		auto* consolePtr = Core::GetSystem<IDE>().FindWindow<IGE_Console>();
		consolePtr->PushMessage(message, textColor);
	}

	void Log_Warning(std::string_view message, const vec4& textColor)
	{
		auto* consolePtr = Core::GetSystem<IDE>().FindWindow<IGE_Console>();
		consolePtr->PushWarning(message, textColor);
	}

	void Log_Error(std::string_view message, const vec4& textColor)
	{
		auto* consolePtr = Core::GetSystem<IDE>().FindWindow<IGE_Console>();
		consolePtr->PushError(message, textColor);
	}

	string ComposeMessage(const char* fmt, ...)
	{
		string returnValue{};
		va_list args1;
		va_start(args1, fmt);
		va_list args2;
		va_copy(args2, args1);
		const int len = vsnprintf(NULL, 0, fmt, args1);
		vector<char> buf{};
		buf.resize(len + 1);
		va_end(args1);
		vsnprintf(buf.data(), buf.size(), fmt, args2);
		va_end(args2);

		for (auto& c : buf)
			returnValue.push_back(std::move(c));
		return returnValue;
	}
}
