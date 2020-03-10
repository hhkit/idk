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
		ServerConnectionManager(int clientId, Server& server);
		~ServerConnectionManager();

		using ConnectionManager::CreateMessage;
		using ConnectionManager::SendMessage;
		using ConnectionManager::GetManager;

		template<typename MessageType, typename Func, typename = sfinae<std::is_invocable_v<Func, MessageType&>>>
		void Subscribe(Func&& func) { Subscribe2<MessageType>(std::forward<Func>(func)); }

		seconds GetRTT() const override;
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
		yojimbo::Message* CreateMessage(size_t id) override;
		void SendMessage(yojimbo::Message* message, GameChannel delivery_mode) override;
		BaseSubstreamManager* GetManager(size_t substream_type_id) override;
		void InstantiateSubmanagers();
		template<typename RealSubstreamManager> RealSubstreamManager& AddSubstreamManager();
	};
}