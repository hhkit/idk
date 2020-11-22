#pragma once
#include <idk.h>
#include <network/ConnectionManager.h>
#include <network/ClientConnectionManager.inl>
#include <network/ServerConnectionManager.inl>
#undef SendMessage
#include <steam/isteamnetworkingsockets.h>
#include <steam/isteamnetworkingutils.h>

namespace idk
{
	template<typename T>
	inline void ConnectionManager::SendMessage(T&& message, GameChannel delivery_mode)
	{
		WriteStream stream{ out_message_buffer };

		stream.SerializeUInt8(MessageID<std::decay_t<T>>);
		if (message.SerializeInternal(stream))
		{
			auto sz = stream.GetSize();
			auto* msg = SteamNetworkingUtils()->AllocateMessage(sz);
			std::memcpy(msg->m_pData, out_message_buffer, sz);
			msg->m_conn = handle;
			switch (delivery_mode)
			{
			case GameChannel::FASTEST_GUARANTEED:
				msg->m_nFlags |= k_nSteamNetworkingSend_Reliable;
				break;
			case GameChannel::RELIABLE:
				msg->m_nFlags |= k_nSteamNetworkingSend_Reliable;
				break;
			case GameChannel::UNRELIABLE:
				msg->m_nFlags |= k_nSteamNetworkingSend_Unreliable;
				break;
			default:
				break;
			}
			
			SendMessage(msg);
		}
	}
	template<typename T, typename Func, typename>
	inline void ConnectionManager::CreateAndSendMessage(GameChannel channel, Func&& func)
	{
		T msg;
		func(msg);
		SendMessage(msg, channel);
	}
	template<typename Manager>
	inline Manager* ConnectionManager::GetManager()
	{
		return static_cast<Manager*>(GetManager(Manager::type));
	}
}