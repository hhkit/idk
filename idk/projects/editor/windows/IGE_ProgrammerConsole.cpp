#include "pch.h"
#include <imgui/imgui.h>
#include <imgui/imgui_internal.h>

#include <core/Scheduler.h>
#include <debug/LogSingleton.h>
#include "IGE_ProgrammerConsole.h"

namespace idk
{
	IGE_ProgrammerConsole::IGE_ProgrammerConsole()
		: IGE_IWindow{ "Programmer", true, ImVec2(500.0f, 750.0f) }
	{
		window_flags = ImGuiWindowFlags_MenuBar;
		for (unsigned i = 0; i < s_cast<unsigned>(LogPool::COUNT); ++i)
		{
			LogSingleton::Get().SignalFor(s_cast<LogPool>(i)).Listen(
				[this, i](LogLevel level, time_point time, string_view preface, string_view message)
			{
				constexpr auto format_time = [](time_point t) -> string
				{
					auto program_start = Core::GetScheduler().GetProgramStart();
					auto time_dif = t - program_start;
					auto ms = duration_cast<std::chrono::milliseconds>(time_dif).count() % 1000;
					auto s = duration_cast<std::chrono::seconds>(time_dif).count() % 60;
					auto m = duration_cast<std::chrono::minutes>(time_dif).count() % 60;
					auto h = duration_cast<std::chrono::hours>(time_dif).count();
					char buf[32];
					sprintf_s(buf, "%d.%.2d.%.2lld.%.3lld", h, m, s, ms);
					return string{ buf };
				};

				switch (level)
				{
				case LogLevel::INFO:    messages[i].emplace_back(Message { .preface = string{ "[LOG] "   } + format_time(time) , .message = string{ preface } + "\t" + string {message} }); break;
				case LogLevel::WARNING: messages[i].emplace_back(Message { .preface = string{ "[WARN] "  } + format_time(time) , .message = string{ preface } + "\t" + string {message} }); break;
				case LogLevel::ERR:     messages[i].emplace_back(Message { .preface = string{ "[ERROR] " } + format_time(time) , .message = string{ preface } + "\t" + string {message} }); break;
				case LogLevel::FATAL:   messages[i].emplace_back(Message { .preface = string{ "[FATAL] " } + format_time(time) , .message = string{ preface } + "\t" + string {message} }); break;
				}

				new_message_received = true;
			}
			);
		}
	}

	void IGE_ProgrammerConsole::BeginWindow()
	{

	}

	void IGE_ProgrammerConsole::Update()
	{
		ImGui::BeginMenuBar();
		if (ImGui::Button("Clear")) {
			clear_messages_called = true;
		}

		
		bool value_changed = ImGui::InputTextEx("##ToolBarSearchBar", NULL, textFilter.InputBuf, IM_ARRAYSIZE(textFilter.InputBuf), ImVec2{ ImGui::GetWindowContentRegionWidth() * 0.2f,ImGui::GetFrameHeight() - 2 }, ImGuiInputTextFlags_None);
		if (value_changed)
			textFilter.Build();

		ImVec4& activeCol = ImGui::GetStyle().Colors[ImGuiCol_ButtonActive];
		ImVec4& nonactiveCol = ImGui::GetStyle().Colors[ImGuiCol_Button];

		auto make_button = [&](LogPool pool, string_view name)
		{
			ImGui::PushStyleColor(ImGuiCol_Button, curr_pool == pool ? activeCol : nonactiveCol);
			if (ImGui::Button(name.data(), ImVec2{ 60.0f,0.0f }))
			{
				curr_pool = pool;
				new_message_received = true;
			}
			ImGui::PopStyleColor();
		};

		make_button(LogPool::ANY, "All");
		make_button(LogPool::SYS, "System");
		make_button(LogPool::PHYS, "Physics");
		make_button(LogPool::GFX, "Graphics");
		make_button(LogPool::ANIM, "Animation");
		make_button(LogPool::MONO, "Mono");
		make_button(LogPool::GAME, "Game");
		make_button(LogPool::EDIT, "Editor");

		ImGui::EndMenuBar();

		if (clear_messages_called) {
			ClearMessages();
			clear_messages_called = false;
			return;
		}
		
		for (auto& message : messages[s_cast<unsigned>(curr_pool)])
			PrintMessage(message);

		if (new_message_received) {
			ImGui::SetScrollHereY(1.0f);
			new_message_received = false;
		}
	}


	void IGE_ProgrammerConsole::ClearMessages()
	{

		for (auto& buffer : messages)
			buffer.erase(buffer.begin(), buffer.end());
	}

	void IGE_ProgrammerConsole::PrintMessage(const Message& message)
	{
//		if (!textFilter.PassFilter(message.text.c_str()))
//			return;

		ImGui::PushStyleColor(ImGuiCol_Text, vec4{0.8f});
		ImGui::TextUnformatted(message.preface.data());
		ImGui::PopStyleColor();

		ImGui::SameLine();
		ImGui::SetCursorPosX(100.0f);
		//Display main text
		ImGui::PushTextWrapPos();
		ImGui::TextUnformatted(message.message.data());
		ImGui::PopTextWrapPos();

		ImGui::Separator();

	}
}