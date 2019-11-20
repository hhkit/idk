#pragma once
#include <idk.h>
#include <ds/circular_buffer.h>
#include <editor/windows/IGE_IWindow.h>

namespace idk
{

	
	class IGE_ProgrammerConsole : public IGE_IWindow
	{
	public:
		IGE_ProgrammerConsole();

		virtual void BeginWindow() override;
		virtual void Update() override;
		~IGE_ProgrammerConsole();
	private:
		struct Message { string preface; string message; };
		using MessageBuffer = circular_buffer<Message, 512>;
		array<MessageBuffer, s_cast<unsigned>(LogPool::COUNT)>  messages;
		array<SignalBase::SlotId, s_cast<unsigned>(LogPool::COUNT)>  registered_signals;
		
		void ClearMessages();
		void PrintMessage(const Message& message);

		bool clear_messages_called = false;
		bool new_message_received = false;
		LogPool curr_pool{};
		ImGuiTextFilter textFilter{};
		
	};
}