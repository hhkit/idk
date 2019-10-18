//////////////////////////////////////////////////////////////////////////////////
//@file		IGE_Console.h
//@author	Muhammad Izha B Rahim
//@param	Email : izha95\@hotmail.com
//@date		17 OCT 2019
//@brief	

/*
Logs logs into a log.
*/
//////////////////////////////////////////////////////////////////////////////////



#pragma once

#include <editor/windows/IGE_IWindow.h>

namespace idk
{
	
	enum ConsoleMessageType_ {
		ConsoleMessageType_Message,
		ConsoleMessageType_Warning,
		ConsoleMessageType_Error
	};

	struct ConsoleMessage {
		//int id;
		ConsoleMessageType_ type = ConsoleMessageType_Message;
		string message{};
		float rgb[3]	{ 1.0f,1.0f,1.0f };

	};

	class IGE_Console : public IGE_IWindow
	{
	public:
		IGE_Console();

		virtual void BeginWindow() override;
		virtual void Update() override;

		void PushMessage(const string& message, float r = 1.0f,float g = 1.0f, float b = 1.0f);
		void PushMessage(const char* message, float r = 1.0f,float g = 1.0f, float b = 1.0f);
		void PushMessage(const ConsoleMessage&);
		string ComposeMessage(const char* fmt, ...); //Same syntax as printf

		void ClearMessages();

		bool display_messages	= true;
		bool display_warnings	= true;
		bool display_errors		= true;

	private:
		

		vector<ConsoleMessage> messages;

		void PrintMessage(const ConsoleMessage& message);

	};
}