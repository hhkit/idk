#pragma once
#include <network/SubstreamManager.h>
#include <res/ResourceHandle.h>
#include <network/network.h>

namespace idk
{
	class EventInstantiatePrefabMessage;
	class EventTransferOwnershipMessage;
	class EventLoadLevelMessage;
	class EventInvokeRPCMessage;

	class EventManager
		: public SubstreamManager<EventManager>
	{
	public:
		void SubscribeEvents(ClientConnectionManager& client) override;
		void SubscribeEvents(ServerConnectionManager& server) override;

		void SendTestMessage(int i);
		static Handle<GameObject> BroadcastInstantiatePrefab(RscHandle<Prefab> prefab, opt<vec3> position, opt<quat> quaternion = {});
		static void SendTransferOwnership(Handle<ElectronView> transfer, Host target_host);
		static void BroadcastLoadLevel(RscHandle<Scene> scene);
		static void BroadcastRPC(Handle<ElectronView> ev, string_view method_name, span<vector<unsigned char>> buffer);
	private:
		void OnInstantiatePrefabEvent(EventInstantiatePrefabMessage* message);
		void OnTransferOwnershipEvent(EventTransferOwnershipMessage* message);
		void OnLoadLevelMessage(EventLoadLevelMessage*);
		void OnInvokeRPCMessage(EventInvokeRPCMessage*);
	};
}