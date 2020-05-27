#pragma once
#include <vector>
#include <optional>
#include "Address.h"

namespace idk
{
	static constexpr Address broadcast_address = Address{255U,255U,255U,255U};

	struct Message
	{
		Address dest;
		Address src;
		std::vector<unsigned char> buffer;
	};

	class Socket
	{
	public:

		virtual ~Socket() = default;

		virtual void Bind(unsigned short port) = 0;
		virtual void Bind(const Address& addr) = 0;
		virtual void Listen() = 0;
		virtual void EnableBroadcast() = 0;

		virtual void Send(const Message&) = 0;
		virtual std::optional<Message> Receive() = 0;
	};
}