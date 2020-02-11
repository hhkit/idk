#include "stdafx.h"
#include "pch_common.h"
#include <yojimbo/yojimbo.h>

#include "NetworkSystem.h"

#include <app/Application.h>
#include <network/Server.h>
#include <network/Client.h>
#include <network/ServerConnectionManager.h>
#include <network/ClientConnectionManager.h>
#include <network/IDManager.h>
#include <network/ElectronView.h>
#include <network/EventDataBlockFrameNumber.h>
#include <core/GameState.h>
namespace idk
{
	NetworkSystem::NetworkSystem() = default;
	NetworkSystem::~NetworkSystem() = default;

	void NetworkSystem::InstantiateServer(const Address& d)
	{
		ResetNetwork();
		frame_counter = 0;
		lobby = std::make_unique<Server>(Address{d.a,d.b,d.c,d.d, server_listen_port});
		lobby->OnClientConnect += [this](int clientid)
		{
			server_connection_manager[clientid] = std::make_unique<ServerConnectionManager>(clientid, *lobby);
			server_connection_manager[clientid]->CreateAndSendMessage<EventDataBlockFrameNumber>(GameChannel::RELIABLE, [this](EventDataBlockFrameNumber& msg)
				{
					msg.frame_count = frame_counter;
				});
		};
		lobby->OnClientDisconnect += [this](int clientid)
		{
			server_connection_manager[clientid].reset();
		};
		id_manager = std::make_unique<IDManager>();
	}

	void NetworkSystem::ConnectToServer(const Address& d)
	{
		ResetNetwork();
		frame_counter = 0;
		client = std::make_unique<Client>(Address{ d.a,d.b,d.c,d.d, server_listen_port });
		client->OnConnectionToServer += [this]()
		{
			client_connection_manager = std::make_unique<ClientConnectionManager>(*client);
			client_connection_manager->Subscribe<EventDataBlockFrameNumber>([this](EventDataBlockFrameNumber* event)
				{
					frame_counter = event->frame_count;

					auto frames_late = static_cast<int>(std::chrono::duration<float, std::milli>(client->GetRTT()) / Core::GetRealDT()) / 2; // attempt to synchronize frame time with the server using half rtt
					frame_counter += frames_late;
				});
		};
		client->OnDisconnectionFromServer += [this]()
		{
			client_connection_manager.reset();
		};
		id_manager = std::make_unique<IDManager>();
	}

	bool NetworkSystem::IsHost()
	{
		return static_cast<bool>(lobby);
	}

	SeqNo NetworkSystem::GetSequenceNumber() const
	{
		return frame_counter;
	}

	ConnectionManager* NetworkSystem::GetConnectionTo(Host host)
	{
		switch (host)
		{
		case Host::SERVER:  return client_connection_manager.get();
		case Host::CLIENT0: return server_connection_manager[0].get();
		case Host::CLIENT1: return server_connection_manager[1].get();
		case Host::CLIENT2: return server_connection_manager[2].get();
		case Host::CLIENT3: return server_connection_manager[3].get();
		case Host::CLIENT_MAX:
		{
			if (client_connection_manager)
				return client_connection_manager.get();
			for (auto& elem : server_connection_manager)
				if (elem)
					return elem.get();
		}
		}
		return nullptr; // no connection found
	}

	void NetworkSystem::ReceivePackets()
	{
		if (frame_counter != 0xFFFF)
			++frame_counter;
		else
			frame_counter = 0;
		LOG_TO(LogPool::NETWORK, "Frame: %d ", (int) frame_counter);

		if (lobby)
		{
			lobby->ReceivePackets();
		}
		if (client)
		{
			client->ReceivePackets();
		}
	}

	void NetworkSystem::SendPackets()
	{
		if (lobby)
			lobby->SendPackets();

		if (client)
			client->SendPackets();
	}

	void NetworkSystem::RespondToPackets()
	{
		// call static
		for (auto& elem : frame_start_callbacks)
			elem.callback();
		
		// per client
		if (client_connection_manager)
			client_connection_manager->FrameStartManagers();
		for (auto& elem : server_connection_manager)
			if (elem)
				elem->FrameStartManagers();
	}

	void NetworkSystem::PreparePackets()
	{
		// call static
		for (auto& elem : frame_end_callbacks)
			elem.callback();

		// per client
		if (client_connection_manager)
			client_connection_manager->FrameEndManagers();
		for (auto& elem : server_connection_manager)
			if (elem)
				elem->FrameEndManagers();
	}

	void NetworkSystem::Init()
	{
		InitializeYojimbo();
	}

	void NetworkSystem::LateInit()
	{
		for (auto& device : Core::GetSystem<Application>().GetNetworkDevices())
		{
			if(device.ip_addresses.size())
				LOG_TO(LogPool::NETWORK, "Found %s device (%s) with address %s.", device.name.c_str(), device.fullname.c_str(), string{ device.ip_addresses.front() }.c_str());
		}
	}

	void NetworkSystem::EarlyShutdown()
	{
		ResetNetwork();
	}

	void NetworkSystem::Shutdown()
	{
		ShutdownYojimbo();
	}
	void NetworkSystem::ResetNetwork()
	{
		for (auto& elem : server_connection_manager)
			elem.reset();
		lobby.reset();
		client_connection_manager.reset();
		client.reset();
		id_manager.reset();

		// network ids no longer relevant
		for (auto& elem : Core::GetGameState().GetObjectsOfType<ElectronView>())
			elem.network_id = 0;
	}
}