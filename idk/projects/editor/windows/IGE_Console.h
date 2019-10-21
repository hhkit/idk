//////////////////////////////////////////////////////////////////////////////////
//@file		IGE_Console.h
//@author	Muhammad Izha B Rahim
//@param	Email : izha95\@hotmail.com
//@date		20 OCT 2019
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
		//const time_point	time_created	= Clock::now();
		ConsoleMessageType_ type			= ConsoleMessageType_Message;
		string				text			= {};
		vec4				color			= { 1.0f,1.0f,1.0f,1.0f };

	};

	class IGE_Console : public IGE_IWindow
	{
	public:
		IGE_Console();

		virtual void BeginWindow() override;
		virtual void Update() override;

		void PushMessage(string_view message	, const vec4& color = { 1.0f,1.0f,1.0f,1.0f });
		void PushWarning(string_view message	, const vec4& color = { 1.0f,1.0f,1.0f,1.0f });
		void PushError	(string_view message	, const vec4& color = { 1.0f,1.0f,1.0f,1.0f });
		void PushRaw	(string_view message	, ConsoleMessageType_ type, const vec4& color = { 1.0f,1.0f,1.0f,1.0f });


		bool display_messages	= true;
		bool display_warnings	= true;
		bool display_errors		= true;
		//bool display_timestamp	= false;

	private:
		

		vector<ConsoleMessage> messages;
		int		num_of_messages			= 0;
		int		num_of_warnings			= 0;
		int		num_of_errors			= 0;
		vec4	message_header_color	= { 1.0f,1.0f,1.0f,1.0f };
		vec4	warning_header_color	= { 1.0f,1.0f,0.0f,1.0f };
		vec4	error_header_color		= { 1.0f,0.0f,0.0f,1.0f };

		void PushConsoleMessage(ConsoleMessage&&); //Use std::move(lValue)
		void PrintMessage(const ConsoleMessage& message,int msgNo);
		void ClearMessages();

		bool new_message_received		= false;
		bool clear_messages_called		= false;
		float start_text_posX			= 0;
		ImGuiTextFilter textFilter{};

	};
}