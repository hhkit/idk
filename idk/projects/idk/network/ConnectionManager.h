#pragma once
#include <idk.h>
#include <network/GameConfiguration.h>
#include <network/network.h>
#include <network/Message.h>
#include <steam/steamnetworkingtypes.h>

#undef SendMessage
namespace idk
{
	class BaseSubstreamManager;

	class ConnectionManager
	{
	public:
		ConnectionManager(HSteamNetConnection handle) : handle{ handle } {}
		
		HSteamNetConnection GetHandle() const;
		seconds GetRTT() const;
		virtual Host GetConnectedHost() const = 0;
		virtual ~ConnectionManager() = default;

		virtual void FrameStartManagers() = 0;
		virtual void FrameEndManagers() = 0;

		//template<typename T> T* CreateMessage() { return static_cast<T*>(CreateMessage(MessageID<T>)); }
		template<typename T> void SendMessage(T&& message, GameChannel delivery_mode);

		template<typename T, typename Func, typename = sfinae<std::is_invocable_v<Func, T&>>> 
		void CreateAndSendMessage(GameChannel channel, Func&& func);

		template<typename Manager> Manager* GetManager();

		virtual void ReceiveMessages() = 0;

	protected:
		//virtual yojimbo::Message* CreateMessage(size_t id) = 0;
		virtual void SendMessage(SteamNetworkingMessage_t* message) = 0;
		virtual class BaseSubstreamManager* GetManager(size_t substream_type_id) = 0;

		std::byte out_message_buffer[k_cbMaxSteamNetworkingSocketsMessageSizeSend];
		std::byte in_message_buffer[k_cbMaxSteamNetworkingSocketsMessageSizeSend];
		SteamNetworkingMessage_t* in_messages[512];

		HSteamNetConnection handle;
	};

}
