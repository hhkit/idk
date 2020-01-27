#include "stdafx.h"
#include "ServerConnectionManager.h"
#include <network/Server.h>
#include <network/SubstreamManager.h>
#include <network/EventManager.h>
#include <network/GhostManager.h>

#undef SendMessage

namespace idk
{
	template<typename RealSubstreamManager>
	inline RealSubstreamManager& ServerConnectionManager::AddSubstreamManager()
	{
		static_assert(has_tag_v<RealSubstreamManager, SubstreamManager>, "Must inherit from CRTP template SubstreamManager<>!");
		auto& ptr = substream_managers.emplace_back(std::make_unique<RealSubstreamManager>());
		ptr->SetConnectionManager(this);
		ptr->SubscribeEvents(*this);
		return static_cast<RealSubstreamManager&>(*ptr);
	}

	ServerConnectionManager::ServerConnectionManager(int _clientID, Server& _server)
		: clientID{_clientID}, server { _server }
	{
		InstantiateSubmanagers();
	}

	ServerConnectionManager::~ServerConnectionManager()
	{
		for (const auto& [type, slot] : OnMessageReceived_slots)
			server.OnMessageReceived[clientID][(int)type].Unlisten(slot);
	}

	yojimbo::Message* ServerConnectionManager::CreateMessage(size_t id)
	{
		return server.CreateMessage(clientID, static_cast<int>(id));
	}

	void ServerConnectionManager::SendMessage(yojimbo::Message* message, bool guarantee_delivery)
	{
		server.SendMessage(clientID, message, guarantee_delivery);
	}

	BaseSubstreamManager* ServerConnectionManager::GetManager(size_t substream_type_id)
	{
		for (auto& elem : substream_managers)
		{
			if (elem->GetManagerType() == substream_type_id)
				return elem.get();
		}
		return nullptr;
	}

	void ServerConnectionManager::InstantiateSubmanagers()
	{
		AddSubstreamManager<EventManager>();
		AddSubstreamManager<GhostManager>();
	}
}
