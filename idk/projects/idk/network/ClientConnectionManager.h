#pragma once
#include <idk.h>
#include <network/ConnectionManager.h>
namespace idk
{
	class Client;
	class BaseSubstreamManager;

	class ClientConnectionManager
		: public ConnectionManager
	{
	public:
		ClientConnectionManager(Client& client);
		~ClientConnectionManager();

		using ConnectionManager::CreateMessage;
		using ConnectionManager::SendMessage;
		using ConnectionManager::GetManager;

		template<typename MessageType, typename Func, typename = sfinae<std::is_invocable_v<Func, MessageType*>>>
		void Subscribe(Func&& func);

		void FrameStartManagers() override;
		void FrameEndManagers() override;
	private:
		struct EventSlot { unsigned type; SignalBase::SlotId slot; };
		Client& client;
		vector<EventSlot> OnMessageReceived_slots;
		vector<unique_ptr<BaseSubstreamManager>> substream_managers;

		yojimbo::Message* CreateMessage(size_t id) override;
		void SendMessage(yojimbo::Message* message, bool guarantee_delivery) override;
		class BaseSubstreamManager* GetManager(size_t substream_type_id) override;

		void InstantiateSubmanagers();
		template<typename RealSubstreamManager> RealSubstreamManager& AddSubstreamManager();
	};

}