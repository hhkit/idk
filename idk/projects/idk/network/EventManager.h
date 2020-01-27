#pragma once
#include <network/SubstreamManager.h>
#include <res/ResourceHandle.h>

namespace idk
{
	class EventManager
		: public SubstreamManager<EventManager>
	{
	public:
		void SubscribeEvents(ClientConnectionManager& client) override;
		void SubscribeEvents(ServerConnectionManager& server) override;

		void SendTestMessage(int i);
		void SendInstantiatePrefabEvent(RscHandle<Prefab> prefab, opt<vec3> position, opt<quat> quaternion);
	private:
		void OnInstantiatePrefabEvent(EventInstantiatePrefabMessage* message);
	};
}