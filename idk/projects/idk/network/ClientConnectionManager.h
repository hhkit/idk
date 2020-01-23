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

		template<typename MessageType, typename Func, typename = sfinae<std::is_invocable_v<Func, MessageType*>>>
		void Subscribe(Func&& func);

		template<typename T> T*   CreateMessage();
		template<typename T> void SendMessage(T* message, bool guarantee_delivery);

		// get relevant substream manager
		template<typename Manager> Manager* GetManager();
	private:
		struct EventSlot { unsigned type; SignalBase::SlotId slot; };
		Client& client;
		vector<EventSlot> OnMessageReceived_slots;
		vector<unique_ptr<BaseSubstreamManager>> substream_managers;

		void InstantiateSubmanagers();
		template<typename RealSubstreamManager> RealSubstreamManager& AddSubstreamManager();
	};

}