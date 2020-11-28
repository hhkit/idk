#pragma once
#include <idk.h>
#include <event/Signal.h>
#include <network/ConnectionManager.h>
namespace idk
{
	class Server;
	class BaseSubstreamManager;

	class ServerConnectionManager
		: public ConnectionManager
	{
	public:
		ServerConnectionManager(int clientId, Server& server, HSteamNetConnection handle);
		~ServerConnectionManager();

		using ConnectionManager::SendMessage;
		using ConnectionManager::GetManager;

		template<typename MessageType, typename Func, typename = sfinae<std::is_invocable_v<Func, MessageType&>>>
		void Subscribe(Func&& func) { Subscribe2<MessageType>(std::forward<Func>(func)); }

		Host GetConnectedHost() const override;
		void FrameStartManagers() override;
		void FrameEndManagers() override;

	private:
		struct EventSlot { unsigned type; SignalBase::SlotId slot; };

		Server& server;
		const int clientID;
		vector<EventSlot> OnMessageReceived_slots;
		vector<unique_ptr<BaseSubstreamManager>> substream_managers;

		template<typename MessageType, typename Func>
		void Subscribe2(Func&& func);
		BaseSubstreamManager* GetManager(size_t substream_type_id) override;
		void InstantiateSubmanagers();
		template<typename RealSubstreamManager> RealSubstreamManager& AddSubstreamManager();

		void SendMessage(SteamNetworkingMessage_t* message) override;
		void ProcessMessage(uint8_t type, ReadStream& stream) override;
	};
}