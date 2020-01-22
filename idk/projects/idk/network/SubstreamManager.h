#pragma once
#include <idk.h>
namespace idk
{
	class ClientConnectionManager;
	class ServerConnectionManager;

	class BaseSubstreamManager
	{
	public:
		virtual ~BaseSubstreamManager() = default;
		virtual void SubscribeEvents(ClientConnectionManager&) = 0;
		virtual void SubscribeEvents(ServerConnectionManager&) = 0;
	};

	template<typename T>
	class SubstreamManager
		: public BaseSubstreamManager
	{};
}