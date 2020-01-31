#pragma once
#include <network/SubstreamManager.h>
#include <res/ResourceHandle.h>
#include <network/network.h>

namespace idk
{
	class EventInstantiatePrefabMessage;
	class EventTransferOwnershipMessage;
	class EventLoadLevelMessage;

	class EventManager
		: public SubstreamManager<EventManager>
	{
	public:
		void SubscribeEvents(ClientConnectionManager& client) override;
		void SubscribeEvents(ServerConnectionManager& server) override;

		void SendTestMessage(int i);
		static void BroadcastInstantiatePrefab(RscHandle<Prefab> prefab, opt<vec3> position, opt<quat> quaternion = {});
		static void SendTransferOwnership(Handle<ElectronView> transfer, Host target_host);
		static void BroadcastLoadLevel(RscHandle<Scene> scene);
	private:
		void OnInstantiatePrefabEvent(EventInstantiatePrefabMessage* message);
		void OnTransferOwnershipEvent(EventTransferOwnershipMessage* message);
		void OnLoadLevelMessage(EventLoadLevelMessage*);
	};
}