#pragma once
#include <network/Socket.h>
#include <WinSock2.h>
namespace idk::win
{
	class WindowsSocket
		: public Socket
	{
	public:
		WindowsSocket();
		~WindowsSocket();

		void Bind(unsigned short port) final;
		void Bind(const Address& addr) final;
		void Listen() final;
		void EnableBroadcast() final;
		void Send(const Message&) final;
		std::optional<Message> Receive() final;
	private:
		SOCKET skt_{};
	};
}