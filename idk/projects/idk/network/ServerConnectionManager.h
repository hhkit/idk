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

		template<typename Func, typename = sfinae<std::is_invocable_v<Func, int, yojimbo::Message*>>>
		void Subscribe(GameMessageType type, Func&& func);
	private:
		struct EventSlot { GameMessageType type; SignalBase::SlotId slot; };
		Server& server;
		const int clientID;
		vector<EventSlot> OnMessageReceived_slots;
		vector<unique_ptr<BaseSubstreamManager>> substream_managers;

		void InstantiateSubmanagers();
		template<typename RealSubstreamManager> RealSubstreamManager& AddSubstreamManager();
	};
}