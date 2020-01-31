#pragma once
#include <yojimbo/yojimbo.h>
namespace idk
{
	static const uint8_t DEFAULT_PRIVATE_KEY[yojimbo::KeyBytes] = { 0 };

	using NetworkMessageTuple = std::tuple<
		class TestMessage
	,	class EventInstantiatePrefabMessage
	,	class EventLoadLevelMessage
	,	class EventTransferOwnershipMessage
	,	class EventInvokeRPCMessage
	,	class GhostMessage
	,	class MoveClientMessage
	,	class MoveReturnMessage
	>;

	static constexpr auto MessageCount = std::tuple_size_v<NetworkMessageTuple>;
	template<typename T>
	static constexpr auto MessageID = index_in_tuple_v<T, NetworkMessageTuple>;

	enum class GameChannel {
		RELIABLE,
		UNRELIABLE,
		FASTEST_GUARANTEED,
		COUNT
	};

	struct GameConfiguration
		: yojimbo::ClientServerConfig
	{
		static constexpr auto MAX_CLIENTS = 8;
		GameConfiguration();
	};
}