#pragma once
#include <idk.h>
#include <event/Signal.h>
#include <network/GameConfiguration.h>
namespace idk
{
	class Client;
	class BaseSubstreamManager;

	class ClientConnectionManager
	{
	public:
		ClientConnectionManager(Client& client);
		~ClientConnectionManager();

		template<typename Func, typename = sfinae<std::is_invocable_v<Func, yojimbo::Message*>>>
		void Subscribe(GameMessageType type, Func&& func);
	private:
		struct EventSlot { GameMessageType type; SignalBase::SlotId slot; };
		Client& client;
		vector<EventSlot> OnMessageReceived_slots;
		vector<unique_ptr<BaseSubstreamManager>> substream_managers;

		void InstantiateSubmanagers();
		template<typename RealSubstreamManager> RealSubstreamManager& AddSubstreamManager();
	};

}