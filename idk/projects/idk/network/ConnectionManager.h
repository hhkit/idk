#pragma once
#include <yojimbo/yojimbo.h>
#include <idk.h>
#include <core/ISystem.h>
#include <network/GameConfiguration.h>
namespace idk
{
	class ConnectionManager
		: public ISystem
	{
	public:
		~ConnectionManager();
		// if index is in the range [0, MAX_CLIENTS), fetches the corresponding from the server connection manager
		// if index is MAX_CLIENTS, will grab the client's manager
		template<typename T> T* GetServerManager(size_t index);
		template<typename T> T* GetClientManager();
		bool IsHost();

	private:
		std::unique_ptr<class ClientConnectionManager> client_connection_manager;
		std::unique_ptr<class ServerConnectionManager> server_connection_manager[GameConfiguration::MAX_CLIENTS];
		void Init();
		void Shutdown();
	};
}
