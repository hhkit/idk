#pragma once
#include <idk.h>
#include <event/Signal.h>
#include <network/GameConfiguration.h>
namespace idk
{
	class Server;
	class BaseSubstreamManager;

	class ServerConnectionManager
	{
	public:
		ServerConnectionManager(int clientId, Server& server);
		~ServerConnectionManager();

		template<typename MessageType, typename Func, typename = sfinae<std::is_invocable_v<Func, MessageType*>>>
		void Subscribe(Func&& func);

		template<typename T> T* CreateMessage();
		template<typename T> void SendMessage(T* message, bool guarantee_delivery = false);
		template<typename Manager> Manager* GetManager();
	private:
		struct EventSlot { unsigned type; SignalBase::SlotId slot; };
		Server& server;
		const int clientID;
		vector<EventSlot> OnMessageReceived_slots;
		vector<unique_ptr<BaseSubstreamManager>> substream_managers;

		void InstantiateSubmanagers();
		template<typename RealSubstreamManager> RealSubstreamManager& AddSubstreamManager();
	};
}