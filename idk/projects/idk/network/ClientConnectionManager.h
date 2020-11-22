#pragma once
#include <idk.h>
#include <network/ConnectionManager.h>
#include <network/network.h>

namespace idk
{
	class Client;
	class BaseSubstreamManager;

	class ClientConnectionManager
		: public ConnectionManager
	{
	public:
		ClientConnectionManager(Client& client, HSteamNetConnection handle);
		~ClientConnectionManager();

		using ConnectionManager::SendMessage;
		using ConnectionManager::GetManager;

		template<typename MessageType, typename Func, typename = sfinae<std::is_invocable_v<Func, MessageType&>>>
		void Subscribe(Func&& func)
		{
			Subscribe2<MessageType>(std::forward<Func>(func)); //Forward hack cause MSVC bug? in 16.2.3
		}

		void FrameStartManagers() override;
		void FrameEndManagers() override;

		void ReceiveMessages();

	private:
		struct EventSlot { unsigned type; SignalBase::SlotId slot; };
		Client& client;
		vector<EventSlot> OnMessageReceived_slots;
		vector<unique_ptr<BaseSubstreamManager>> substream_managers;

		Host GetConnectedHost() const override;

		template<typename MessageType, typename Func>
		void Subscribe2(Func&& func);
		void SendMessage(SteamNetworkingMessage_t* message) override;
		class BaseSubstreamManager* GetManager(size_t substream_type_id) override;

		void InstantiateSubmanagers();
		template<typename RealSubstreamManager> RealSubstreamManager& AddSubstreamManager();
	};

}