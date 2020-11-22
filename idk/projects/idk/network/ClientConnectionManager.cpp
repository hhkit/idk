#include "stdafx.h"
#include "ClientConnectionManager.h"
#include "SubstreamManager.h"
#include <network/Client.h>
#include <network/ClientConnectionManager.inl>
#include <network/ClientMoveManager.h>
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
		template<typename T> struct NetworkMessageHelper;

		template<typename ... Ts>
		class NetworkMessageHelper<std::tuple<Ts...>>
		{
			static constexpr array<void (*)(Client& client, ReadStream& stream), sizeof...(Ts)> ProcessMessageJT{
				[](Client& client, ReadStream& stream) {
					Ts msg;
					msg.SerializeInternal(stream);
					client.ProcessMessage(&msg, MessageID<Ts>);
				} ...
			};

		public:
			void ProcessMessage(Client& client, uint8_t message_type, ReadStream& stream)
			{
				ProcessMessageJT[message_type](client, stream);
			}
		};
	}

	template<typename RealSubstreamManager>
	inline RealSubstreamManager& ClientConnectionManager::AddSubstreamManager()
	{
		static_assert(has_tag_v<RealSubstreamManager, SubstreamManager>, "Must inherit from CRTP template SubstreamManager<>!");
		auto& ptr = substream_managers.emplace_back(std::make_unique<RealSubstreamManager>());
		ptr->SubscribeEvents(*this);
		ptr->SetConnectionManager(this);
		return static_cast<RealSubstreamManager&>(*ptr);
	}

	ClientConnectionManager::ClientConnectionManager(Client& client, HSteamNetConnection handle)
		: ConnectionManager(handle), client{ client }
	{
		InstantiateSubmanagers();
	}

	ClientConnectionManager::~ClientConnectionManager()
	{
		SteamNetworkingSockets()->CloseConnection(handle, 0, nullptr, false);
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

	Host ClientConnectionManager::GetConnectedHost() const
	{
		return Host::SERVER;
	}

	//yojimbo::Message* ClientConnectionManager::CreateMessage(size_t id)
	//{
	//	constexpr auto message_name_array = detail::NetworkHelper::GenNames();

	//	// if (   id != index_in_tuple_v<GhostMessage, NetworkMessageTuple>
	//	// 	&& id != index_in_tuple_v<GhostAcknowledgementMessage, NetworkMessageTuple>
	//	// 	&& id != index_in_tuple_v<MoveClientMessage, NetworkMessageTuple>
	//	// 	)
	//	// LOG_TO(LogPool::NETWORK, "creating %s message", message_name_array[id].data());
	//	return client.CreateMessage(static_cast<int>(id));
	//}

	void ClientConnectionManager::SendMessage(SteamNetworkingMessage_t* message)
	{
		client.SendMessage(message);
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

	void ClientConnectionManager::ReceiveMessages()
	{
		auto helper = detail::NetworkMessageHelper<NetworkMessageTuple>();

		int count = SteamNetworkingSockets()->ReceiveMessagesOnConnection(handle, in_messages, std::size(in_messages));
		for (int i = 0; i < count; ++i)
		{
			auto* msg = in_messages[i];
			std::memcpy(in_message_buffer, msg->m_pData, msg->m_cbSize);
			ReadStream stream{ in_message_buffer, static_cast<uint32_t>(msg->m_cbSize) };
			uint8_t type;
			stream.SerializeUInt8(type);

			helper.ProcessMessage(client, type, stream);

			msg->Release();
		}
	}
}
