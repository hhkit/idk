#include "stdafx.h"
#include "ClientConnectionManager.h"
#include "SubstreamManager.h"
#include <network/Client.h>
#include <network/EventManager.h>

namespace idk
{
	template<typename RealSubstreamManager>
	inline RealSubstreamManager& ClientConnectionManager::AddSubstreamManager()
	{
		static_assert(has_tag_v<RealSubstreamManager, SubstreamManager>, "Must inherit from CRTP template SubstreamManager<>!");
		auto& ptr = substream_managers.emplace_back(std::make_unique<RealSubstreamManager>());
		ptr->SubscribeEvents(*this);
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

	yojimbo::Message* ClientConnectionManager::CreateMessage(size_t id)
	{
		return client.CreateMessage(static_cast<int>(id));
	}

	void ClientConnectionManager::SendMessage(yojimbo::Message* message, bool guarantee_delivery)
	{
		client.SendMessage(message, guarantee_delivery);
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
		AddSubstreamManager<EventManager>();
	}
}
