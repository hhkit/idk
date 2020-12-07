#include "stdafx.h"
#include "ServerConnectionManager.h"
#include <network/Server.h>
#include <network/SubstreamManager.h>
#include <network/ServerMoveManager.h>
#include <network/EventManager.h>
#include <network/GhostManager.h>
#include <network/NetworkTuple.inl>
#include <meta/variant.inl>
#include <steam/isteamnetworkingsockets.h>
#include <network/IncludeMessages.h>
#undef SendMessage

namespace idk
{
	namespace detail
	{
		template<typename T> struct ServerMessageHelper;

		template<typename ... Ts>
		class ServerMessageHelper<std::tuple<Ts...>>
		{
			static constexpr array<void (*)(Server& server, int clientid, ReadStream& stream), sizeof...(Ts)> ProcessMessageJT{
				[](Server& server, int clientid, ReadStream& stream) {
					Ts msg;
					msg.SerializeInternal(stream);
					server.ProcessMessage(clientid, &msg, MessageID<Ts>);
				} ...
			};

		public:
			void ProcessMessage(Server& server, int clientid, uint8_t message_type, ReadStream& stream) const
			{
				ProcessMessageJT[message_type](server, clientid, stream);
			}
		};
	}

	template<typename RealSubstreamManager>
	inline RealSubstreamManager& ServerConnectionManager::AddSubstreamManager()
	{
		static_assert(has_tag_v<RealSubstreamManager, SubstreamManager>, "Must inherit from CRTP template SubstreamManager<>!");
		auto& ptr = substream_managers.emplace_back(std::make_unique<RealSubstreamManager>());
		ptr->SetConnectionManager(this);
		ptr->SubscribeEvents(*this);
		return static_cast<RealSubstreamManager&>(*ptr);
	}

	ServerConnectionManager::ServerConnectionManager(int _clientID, Server& _server, HSteamNetConnection handle)
		: ConnectionManager(handle), clientID{ _clientID }, server{ _server }
	{
		InstantiateSubmanagers();
	}

	ServerConnectionManager::~ServerConnectionManager()
	{
		SteamNetworkingSockets()->CloseConnection(GetHandle(), 0, nullptr, false);
		for (const auto& [type, slot] : OnMessageReceived_slots)
			server.OnMessageReceived[clientID][(int)type].Unlisten(slot);
	}

	void ServerConnectionManager::FrameStartManagers()
	{
		for (auto& elem : substream_managers)
			elem->NetworkFrameStart();
	}

	void ServerConnectionManager::FrameEndManagers()
	{
		for (auto& elem : substream_managers)
			elem->NetworkFrameEnd();
	}

	Host ServerConnectionManager::GetConnectedHost() const
	{
		return (Host) clientID;
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
		AddSubstreamManager<ServerMoveManager>();
		AddSubstreamManager<EventManager>();
		AddSubstreamManager<GhostManager>();
	}

	void ServerConnectionManager::SendMessage(SteamNetworkingMessage_t* message)
	{
		server.SendMessage(message);
	}

	void ServerConnectionManager::ProcessMessage(uint8_t type, ReadStream& stream)
	{
		static constexpr auto helper = detail::ServerMessageHelper<NetworkMessageTuple>();
		helper.ProcessMessage(server, clientID, static_cast<uint8_t>(type), stream);
	}
}
