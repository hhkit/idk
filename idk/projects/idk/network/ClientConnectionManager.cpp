#include "stdafx.h"
#include "ClientConnectionManager.h"
#include "SubstreamManager.h"
#include <network/Client.h>
#include <network/ClientConnectionManager.inl>
#include <network/ClientMoveManager.h>
#include <network/EventManager.h>
#include <network/GhostManager.h>

#undef SendMessage

namespace idk
{
	template<typename RealSubstreamManager>
	inline RealSubstreamManager& ClientConnectionManager::AddSubstreamManager()
	{
		static_assert(has_tag_v<RealSubstreamManager, SubstreamManager>, "Must inherit from CRTP template SubstreamManager<>!");
		auto& ptr = substream_managers.emplace_back(std::make_unique<RealSubstreamManager>());
		ptr->SubscribeEvents(*this);
		ptr->SetConnectionManager(this);
		return static_cast<RealSubstreamManager&>(*ptr);
	}

	ClientConnectionManager::ClientConnectionManager(Client& _client)
		: client{ _client }
	{
		InstantiateSubmanagers();
	}

	ClientConnectionManager::~ClientConnectionManager()
	{
		for (const auto& [type, slot] : OnMessageReceived_slots)
			client.OnMessageReceived[(int) type].Unlisten(slot);
	}

	void ClientConnectionManager::FrameStartManagers()
	{
		for (auto& elem : substream_managers)
			elem->NetworkFrameStart();
	}

	void ClientConnectionManager::FrameEndManagers()
	{
		for (auto& elem : substream_managers)
			elem->NetworkFrameEnd();
	}

	yojimbo::Message* ClientConnectionManager::CreateMessage(size_t id)
	{
		return client.CreateMessage(static_cast<int>(id));
	}

	void ClientConnectionManager::SendMessage(yojimbo::Message* message, GameChannel delivery_mode)
	{
		client.SendMessage(message, delivery_mode);
	}

	BaseSubstreamManager* ClientConnectionManager::GetManager(size_t substream_type_id)
	{
		for (auto& elem : substream_managers)
		{
			if (elem->GetManagerType() == substream_type_id)
				return elem.get();
		}
		return nullptr;
	}

	void ClientConnectionManager::InstantiateSubmanagers()
	{
		AddSubstreamManager<ClientMoveManager>();
		AddSubstreamManager<EventManager>();
		AddSubstreamManager<GhostManager>();
	}
}
