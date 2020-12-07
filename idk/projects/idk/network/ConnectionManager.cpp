#include "stdafx.h"
#include "ConnectionManager.h"
#include <steam/isteamnetworkingsockets.h>
#include <steam/isteamnetworkingutils.h>

namespace idk
{
	HSteamNetConnection ConnectionManager::GetHandle() const
	{
		return handle;
	}

	seconds ConnectionManager::GetRTT() const
	{
		SteamNetworkingQuickConnectionStatus status;
		SteamNetworkingSockets()->GetQuickConnectionStatus(handle, &status);

		return duration_cast<seconds>(std::chrono::duration<float, std::milli>(status.m_nPing));
	}

	void ConnectionManager::ReceiveMessages()
	{
		int count = SteamNetworkingSockets()->ReceiveMessagesOnConnection(handle, in_messages,
																		  static_cast<int>(std::size(in_messages)));
		for (int i = 0; i < count; ++i)
		{
			auto* msg = in_messages[i];
			std::memcpy(message_buffer, msg->m_pData, msg->m_cbSize);

			using Stream = ReadStream;
			ReadStream stream{ message_buffer, static_cast<uint32_t>(msg->m_cbSize) };
			uint32_t type;
			serialize_int(stream, type, 0, MessageCount);

			ProcessMessage(static_cast<uint8_t>(type), stream);

			msg->Release();
		}
	}

	SteamNetworkingMessage_t* ConnectionManager::CreateMessage(int size, GameChannel delivery_mode)
	{
		auto* msg = SteamNetworkingUtils()->AllocateMessage(size);
		std::memcpy(msg->m_pData, message_buffer, size);
		msg->m_conn = handle;
		switch (delivery_mode)
		{
		case GameChannel::FASTEST_GUARANTEED: msg->m_nFlags |= k_nSteamNetworkingSend_ReliableNoNagle; break;
		case GameChannel::RELIABLE:           msg->m_nFlags |= k_nSteamNetworkingSend_Reliable; break;
		case GameChannel::UNRELIABLE:         msg->m_nFlags |= k_nSteamNetworkingSend_Unreliable; break;
		default: break;
		}
		return msg;
	}
}