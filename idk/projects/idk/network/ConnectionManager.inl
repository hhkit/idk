#pragma once
#include <idk.h>
#include <network/ConnectionManager.h>
#include <network/ClientConnectionManager.inl>
#include <network/ServerConnectionManager.inl>
#undef SendMessage
namespace idk
{
	template<typename T>
	inline void ConnectionManager::SendMessage(T* message, GameChannel delivery_mode)
	{
		SendMessage((yojimbo::Message*) message, delivery_mode);
	}
	template<typename T, typename Func, typename>
	inline void ConnectionManager::CreateAndSendMessage(GameChannel channel, Func&& func)
	{
		auto msg = CreateMessage<T>();
		func(*msg);
		SendMessage(msg, channel);
	}
	template<typename Manager>
	inline Manager* ConnectionManager::GetManager()
	{
		return static_cast<Manager*>(GetManager(index_in_tuple_v<Manager, SubstreamTypes>));
	}
}