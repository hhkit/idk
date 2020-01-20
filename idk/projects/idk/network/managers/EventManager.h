#pragma once
#include <res/ResourceHandle.inl>

namespace idk
{
	class EventInstantiatePrefabPayload;

	class EventManager
	{
	public:
		bool SendEvent(const EventInstantiatePrefabPayload& event);
	};
}