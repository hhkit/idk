#pragma once
#include <yojimbo/yojimbo.h>
#include <idk.h>
#include <network/GameConfiguration.h>
namespace idk
{
	class BaseSubstreamManager;

	class ConnectionManager
	{
	public:
		virtual ~ConnectionManager() = default;

		virtual void FrameStartManagers() = 0;
		virtual void FrameEndManagers() = 0;

		template<typename T> T* CreateMessage() { return static_cast<T*>(CreateMessage(MessageID<T>)); }
		template<typename T> void SendMessage(T* message, GameChannel delivery_mode);
		template<typename Manager> Manager* GetManager();
	protected:
		virtual yojimbo::Message* CreateMessage(size_t id) = 0;
		virtual void SendMessage(yojimbo::Message* message, GameChannel delivery_mode) = 0;
		virtual class BaseSubstreamManager* GetManager(size_t substream_type_id) = 0;
	};

}
