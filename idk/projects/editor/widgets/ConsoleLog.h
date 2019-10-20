//////////////////////////////////////////////////////////////////////////////////
//@file		ConsoleLog.h
//@author	Muhammad Izha B Rahim
//@param	Email : izha95\@hotmail.com
//@date		20 OCT 2019
//@brief	

/*
Wrapper for printing into IGE_Console.
*/
//////////////////////////////////////////////////////////////////////////////////




#pragma once

#include <idk.h>
#include <editor/windows/IGE_Console.h>

namespace idk
{

	namespace console {

	//MAIN FUNCTIONS
	void Log_Message	(std::string_view message, const vec4& textColor = {1.0f,1.0f,1.0f,1.0f});	//Appends a [Header] at the beginning of the Log. Eg: [MSG] Blah Bla.
	void Log_Warning	(std::string_view message, const vec4& textColor = {1.0f,1.0f,1.0f,1.0f});	//Appends a [Header] at the beginning of the Log. Eg: [WRN] Blah Bla.
	void Log_Error		(std::string_view message, const vec4& textColor = {1.0f,1.0f,1.0f,1.0f});	//Appends a [Header] at the beginning of the Log. Eg: [ERR] Blah Bla.

	//UTILITIES
	string ComposeMessage	(const char* fmt, ...); //Same syntax as printf. Converts format into string.
	}
}