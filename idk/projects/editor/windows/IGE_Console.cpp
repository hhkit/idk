//////////////////////////////////////////////////////////////////////////////////
//@file		IGE_Console.cpp
//@author	Muhammad Izha B Rahim
//@param	Email : izha95\@hotmail.com
//@date		17 OCT 2019
//@brief	

/*
Logs logs into a log.
*/
//////////////////////////////////////////////////////////////////////////////////


#include "pch.h"
#include <editor/windows/IGE_Console.h>
#include <iostream>

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
			ClearMessages();
			ImGui::EndMenuBar();
			return;
		}
		if (ImGui::Button("Test Message")) {
			static int i = 1;
			
			PushMessage(ComposeMessage("Hello!\nI am no:%d", i++),0.0f);
		}
		ImGui::EndMenuBar();

		for (int i = 0; i < messages.size(); ++i) {
			PrintMessage(messages[i]);
		}

	}

	void IGE_Console::PushMessage(const string& message, float r, float g, float b)
	{
		ConsoleMessage newMessage{};
		newMessage.message = message;
		newMessage.type = ConsoleMessageType_Message;
		newMessage.rgb[0] = r;
		newMessage.rgb[1] = g;
		newMessage.rgb[2] = b;
		PushMessage(newMessage);
	}

	void IGE_Console::PushMessage(const char* message, float r, float g, float b)
	{
		PushMessage(string(message), r, g, b);
	}

	void IGE_Console::PushMessage(const ConsoleMessage& message)
	{
		messages.push_back(message);
	}

	string IGE_Console::ComposeMessage(const char* fmt, ...)
	{
		string returnValue{};
		va_list args1;
		va_start(args1, fmt);
		va_list args2;
		va_copy(args2, args1);
		const int len = vsnprintf(NULL, 0, fmt, args1);
		vector<char> buf{};
		buf.resize(len+1);
		va_end(args1);
		vsnprintf(buf.data(),  buf.size(), fmt, args2);
		va_end(args2);

		for (auto& c : buf)
			returnValue.push_back(std::move(c));
		return returnValue;
	}

	void IGE_Console::ClearMessages()
	{
		messages.clear();
	}

	void IGE_Console::PrintMessage(const ConsoleMessage& message)
	{
		ImGui::PushID(&message);
		ImVec2 messageWindowSize = {ImGui::GetWindowContentRegionWidth(), ImGui::GetTextLineHeightWithSpacing() * 2.5f };
		ImGuiWindowFlags_ childFlags = ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoFocusOnAppearing;
		ImGui::BeginChild("msg", messageWindowSize, true, childFlags);
		ImGui::TextColored(ImVec4{ message.rgb[0],message.rgb[1] ,message.rgb[2] ,1.0f }, message.message.c_str());
		ImGui::EndChild();
		ImGui::PopID();

	}



}