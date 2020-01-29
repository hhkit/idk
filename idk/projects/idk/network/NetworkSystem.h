#pragma once
#include <yojimbo/yojimbo.h>
#include <core/ISystem.h>
#include <network/Address.h>
#include <network/GameConfiguration.h>
#include <event/Signal.h>
#include <network/network.h>
namespace idk
{
	class Client;
	class Server;
	class ConnectionManager;
	class IDManager;
	class ClientConnectionManager;
	class ServerConnectionManager;

	class NetworkSystem
		: public ISystem
	{
		static constexpr unsigned short server_listen_port = 8080;
		static constexpr unsigned short client_listen_port = 8000;
	public:
		NetworkSystem();
		~NetworkSystem();

		void InstantiateServer(const Address& d);
		void ConnectToServer(const Address& d);

		Client& GetClient() { return *client; }
		Server& GetServer() { return *lobby; }

		bool IsHost();
		ConnectionManager* GetConnectionTo(Host host = Host::ANY);
		template<typename Message, typename InstantiationFunc, typename = sfinae<std::is_invocable_v<InstantiationFunc, Message&>>>
		void BroadcastMessage(GameChannel channel, InstantiationFunc&& func);
		IDManager& GetIDManager() { return *id_manager; }

		void ReceivePackets();
		void SendPackets();

		void RespondToPackets();
		void PreparePackets();

		template<typename ... Objects> void SubscribePacketResponse(void(*fn)(span<Objects...>));
	private:
		struct ResponseCallback {
			void* fn_ptr;
			function<void()> callback;
		};

		unique_ptr<Server> lobby;
		unique_ptr<Client> client;
		unique_ptr<ServerConnectionManager> server_connection_manager[GameConfiguration::MAX_CLIENTS];
		unique_ptr<ClientConnectionManager> client_connection_manager;

		unique_ptr<IDManager> id_manager;

		vector<ResponseCallback> frame_start_callbacks;
		vector<ResponseCallback> frame_end_callbacks;

		void Init() override;
		void LateInit() override;
		void Shutdown() override;

		void ResetNetwork();
	};
}