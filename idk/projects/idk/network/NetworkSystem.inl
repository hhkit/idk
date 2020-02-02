#pragma once
#include <network/NetworkSystem.h>
#include <network/ConnectionManager.inl>

namespace idk
{
	template<typename Message, typename InstantiationFunc, typename>
	inline void NetworkSystem::BroadcastMessage(GameChannel channel, InstantiationFunc&& func)
	{
		for (auto& elem : server_connection_manager)
		{
			if (elem)
				elem->CreateAndSendMessage<Message>(channel, std::forward<InstantiationFunc>(func));
		}
	}
	template<typename ...Objects>
	void NetworkSystem::SubscribePacketResponse(void(*fn)(span<Objects...>))
	{
		for (auto& elem : frame_start_callbacks)
			if (elem.fn_ptr == fn)
				return;

		ResponseCallback callback{ fn, [fn]()
		{
			fn(Core::GetGameState().GetObjectsOfType<Objects>()...);
		} };
		frame_start_callbacks.emplace_back(std::move(callback));
	}
	/*
	template<typename ...Objects>
	void NetworkSystem::SubscribePacketPreparation(void(*fn)(span<Objects...>))
	{
		for (auto& elem : frame_end_callbacks)
			if (elem.fn_ptr == fn)
				return;

		ResponseCallback callback{ fn, [fn]()
		{
			fn(Core::GetGameState().GetObjectsOfType<Objects>()...);
		} };
		frame_end_callbacks.emplace_back(std::move(callback));
	}
	*/
}