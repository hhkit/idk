//////////////////////////////////////////////////////////////////////////////////
//@file		IGE_Console.cpp
//@author	Muhammad Izha B Rahim
//@param	Email : izha95\@hotmail.com
//@date		20 OCT 2019
//@brief	

/*
Logs logs into a log.
*/
//////////////////////////////////////////////////////////////////////////////////


#include "pch.h"
#include <editor/windows/IGE_Console.h>
#include <editorstatic/imgui/imgui_internal.h> //InputTextEx

namespace idk
{

	IGE_Console::IGE_Console()
        : IGE_IWindow{ "Console", false, ImVec2(500.0f, 750.0f) }
    {
		window_flags = ImGuiWindowFlags_MenuBar;
    }

    void IGE_Console::BeginWindow()
    {

    }

	void IGE_Console::Update()
	{
		ImGui::BeginMenuBar();
		if (ImGui::Button("Clear")) {
			clear_messages_called = true;
		}


		bool value_changed = ImGui::InputTextEx("##ToolBarSearchBar", NULL, textFilter.InputBuf, IM_ARRAYSIZE(textFilter.InputBuf), ImVec2{ ImGui::GetWindowContentRegionWidth()*0.2f,ImGui::GetFrameHeight() - 2 }, ImGuiInputTextFlags_None);
		if (value_changed)
			textFilter.Build();

		string msgCount ,wrnCount,errCount = "+99";
		if (num_of_messages < 99)
			msgCount = std::to_string(num_of_messages);
		if (num_of_warnings < 99)
			wrnCount = std::to_string(num_of_warnings);
		if (num_of_errors < 99)
			errCount = std::to_string(num_of_errors);
		string msg = "MSG [" + msgCount + "]";
		string wrn = "WRN [" + wrnCount + "]";
		string err = "ERR [" + errCount + "]";

		ImVec4& activeCol = ImGui::GetStyle().Colors[ImGuiCol_ButtonActive];
		ImVec4& nonactiveCol = ImGui::GetStyle().Colors[ImGuiCol_Button];


		ImGui::PushStyleColor(ImGuiCol_Button, display_messages ? activeCol: nonactiveCol);
		if (ImGui::Button(msg.c_str(), ImVec2{ 60.0f,0.0f })) {
			display_messages = !display_messages;
		}
		ImGui::PopStyleColor();
		ImGui::PushStyleColor(ImGuiCol_Button, display_warnings ? activeCol : nonactiveCol);
		if (ImGui::Button(wrn.c_str(), ImVec2{ 60.0f,0.0f })) {
			display_warnings = !display_warnings;
		}
		ImGui::PopStyleColor();
		ImGui::PushStyleColor(ImGuiCol_Button, display_errors ? activeCol : nonactiveCol);
		if (ImGui::Button(err.c_str(), ImVec2{ 60.0f,0.0f })) {
			display_errors = !display_errors;
		}
		ImGui::PopStyleColor();

		ImGui::EndMenuBar();

		if (clear_messages_called) {
			ClearMessages();
			clear_messages_called = false;
			return;
		}

		for (int i = 0; i < messages.size(); ++i) {
			PrintMessage(messages[i],i);
		}

		if (new_message_received) {
			ImGui::SetScrollHereY(1.0f);
			new_message_received = false;
		}

	}

	void IGE_Console::PushMessage(string_view message, const vec4& color)
	{
		PushRaw(message, ConsoleMessageType_Message, color);
	}

	void IGE_Console::PushWarning(string_view message, const vec4& color)
	{
		PushRaw(message, ConsoleMessageType_Warning, color);

	}

	void IGE_Console::PushError(string_view message, const vec4& color)
	{
		PushRaw(message, ConsoleMessageType_Error, color);

	}

	void IGE_Console::PushRaw(string_view message, ConsoleMessageType_ type, const vec4& color)
	{
		ConsoleMessage newMessage{};
		newMessage.text = message;
		newMessage.type = type;
		newMessage.color = color;
		PushConsoleMessage(std::move(newMessage));
		new_message_received = true;
	}

	void IGE_Console::PushConsoleMessage(ConsoleMessage&& message)
	{
		switch (message.type) {
		case ConsoleMessageType_Message:
			++num_of_messages;
			break;
		case ConsoleMessageType_Warning:
			++num_of_warnings;
			break;
		case ConsoleMessageType_Error:
			++num_of_errors;
			break;
		}
		messages.push_back(std::move(message));
	}

	void IGE_Console::ClearMessages()
	{
		num_of_messages =
		num_of_warnings =
		num_of_errors	= 0;

		start_text_posX = 0;

		messages.clear();
	}

	void IGE_Console::PrintMessage(const ConsoleMessage& message,int msgNo)
	{
		if (!display_messages	&& message.type == ConsoleMessageType_Message)
			return;
		if (!display_warnings	&& message.type == ConsoleMessageType_Warning)
			return;
		if (!display_errors		&& message.type == ConsoleMessageType_Error)
			return;

		if (!textFilter.PassFilter(message.text.c_str()))
			return;

		switch (message.type) {
		case ConsoleMessageType_Message:
			ImGui::PushStyleColor(ImGuiCol_Text, message_header_color);
			ImGui::TextUnformatted("[MSG]");
			ImGui::PopStyleColor();
			break;
		case ConsoleMessageType_Warning:
			ImGui::PushStyleColor(ImGuiCol_Text, warning_header_color);
			ImGui::TextUnformatted("[WRN]");
			ImGui::PopStyleColor();
			break;
		case ConsoleMessageType_Error:
			ImGui::PushStyleColor(ImGuiCol_Text, error_header_color);
			ImGui::TextUnformatted("[ERR]");
			ImGui::PopStyleColor();
			break;
		}
		ImGui::SameLine();
		ImGui::SetCursorPosX(50.0f);

		ImGui::TextUnformatted(string{ "[" + std::to_string(msgNo) + "]" }.c_str());
		ImGui::SameLine();

		if (start_text_posX < ImGui::GetCursorPosX())
			start_text_posX = ImGui::GetCursorPosX();
		ImGui::SetCursorPosX(start_text_posX);

		//Display main text
		ImGui::PushStyleColor(ImGuiCol_Text, message.color);
		ImGui::PushTextWrapPos();
		ImGui::TextUnformatted(message.text.c_str());
		ImGui::PopTextWrapPos();
		ImGui::PopStyleColor();
		
		ImGui::Separator();

	}



}