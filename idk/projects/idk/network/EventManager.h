#pragma once
#include <network/SubstreamManager.h>
#include <res/ResourceHandle.h>
#include <network/network.h>
#include <network/RPCTarget.h>

namespace idk
{
	class EventInstantiatePrefabMessage;
	class EventDataBlockBufferedEvents;
	class EventTransferOwnershipMessage;
	class EventLoadLevelMessage;
	class EventInvokeRPCMessage;
	class EventDestroyObjectMessage;
	 
	class EventManager
		: public SubstreamManager<EventManager>
	{
	public:
		struct BufferedEvents;

		void SubscribeEvents(ClientConnectionManager& client) override;
		void SubscribeEvents(ServerConnectionManager& server) override;

		void SendTestMessage(int i);
		void SendRPC(Handle<ElectronView> ev, string_view method_name, span<vector<unsigned char>> buffer);
		void SendBufferedEvents();

		static void Init();
		static void ResetEventBuffer();
		static Handle<GameObject> BroadcastInstantiatePrefab(RscHandle<Prefab> prefab, opt<vec3> position, opt<quat> quaternion = {});
		static void SendTransferOwnership(Handle<ElectronView> transfer, Host target_host);
		static void BroadcastLoadLevel(RscHandle<Scene> scene);
		static void BroadcastDestroyView(Handle<ElectronView> view);
		static void BroadcastRPC(Handle<ElectronView> ev, RPCTarget target, string_view method_name, span<vector<unsigned char>> buffer);
	private:
		static inline bool scene_changing = false;
		void OnInstantiatePrefabEvent(EventInstantiatePrefabMessage& message);
		void OnTransferOwnershipEvent(EventTransferOwnershipMessage& message);
		void OnDestroyObjectMessage(EventDestroyObjectMessage& message);
		void OnLoadLevelMessage(EventLoadLevelMessage&);
		void OnInvokeRPCMessage(EventInvokeRPCMessage&);
		void OnBufferedEventMessage(EventDataBlockBufferedEvents&);
	};
}