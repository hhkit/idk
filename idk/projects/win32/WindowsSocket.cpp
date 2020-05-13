#include "pch.h"
#include "WindowsSocket.h"

namespace idk::win
{
	WindowsSocket::WindowsSocket()
	{
		skt_ = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
		u_long mode = 1;  // 1 to enable non-blocking socket
		ioctlsocket(skt_, FIONBIO, &mode);
	}

	WindowsSocket::~WindowsSocket()
	{
		closesocket(skt_);
	}

	void WindowsSocket::Bind(unsigned short port)
	{
		sockaddr_in service;
		service.sin_family = AF_INET;
		service.sin_addr.S_un.S_addr = INADDR_ANY;
		service.sin_port = htons(port);

		//----------------------
		// Bind the socket.
		auto iResult = bind(skt_, (SOCKADDR*)&service, sizeof(service));
		if (iResult == SOCKET_ERROR)
			wprintf(L"bind failed with error %u\n", WSAGetLastError());
		else
			wprintf(L"bind returned success\n");
	}

	void WindowsSocket::Listen()
	{
	}

	void WindowsSocket::EnableBroadcast()
	{
		auto val = TRUE;
		setsockopt(skt_, SOL_SOCKET, SO_BROADCAST, (const char*)&val, (int)sizeof(BOOL));
	}

	void WindowsSocket::Send(const Message& msg)
	{
		sockaddr_in dest;
		dest.sin_family = AF_INET;
		dest.sin_addr.S_un.S_un_b.s_b1 = msg.dest.a;
		dest.sin_addr.S_un.S_un_b.s_b2 = msg.dest.b;
		dest.sin_addr.S_un.S_un_b.s_b3 = msg.dest.c;
		dest.sin_addr.S_un.S_un_b.s_b4 = msg.dest.d;

		dest.sin_port = htons(msg.dest.port);

		auto error = sendto(skt_,
			(const char*)msg.buffer.data(),
			msg.buffer.size(),
			0,
			(const sockaddr*) &dest,
			sizeof(dest)
			);

		if (error == SOCKET_ERROR)
		{
			LOG_TO(LogPool::NETWORK, "ERROR %d", WSAGetLastError());
		}
	}

	std::optional<Message> WindowsSocket::Receive()
	{
		struct sockaddr_in sender;
		int sendsize = sizeof(sender);

		static vector<unsigned char> buf;
		buf.resize(65535);

		auto len = recvfrom(skt_, 
			(char*) buf.data(), 
			buf.size(), 
			0, 
			(struct sockaddr*) & sender, 
			&sendsize);

		if (len == SOCKET_ERROR)
		{
			auto err = WSAGetLastError();
			if (err != WSAEWOULDBLOCK)
				LOG_TO(LogPool::NETWORK, "ERROR %d", err);
			return {};
		}
		Message msg;
		msg.src.a = sender.sin_addr.S_un.S_un_b.s_b1;
		msg.src.b = sender.sin_addr.S_un.S_un_b.s_b2;
		msg.src.c = sender.sin_addr.S_un.S_un_b.s_b3;
		msg.src.d = sender.sin_addr.S_un.S_un_b.s_b4;
		msg.src.port = ntohs(sender.sin_port);

		msg.buffer.insert(msg.buffer.end(), &buf[0], &buf[len]);
		return msg;
	}
}