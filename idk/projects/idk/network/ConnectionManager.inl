#pragma once
#include <idk.h>
#include <network/ConnectionManager.h>
#include <network/ClientConnectionManager.inl>
#include <network/ServerConnectionManager.inl>
#undef SendMessage
#include <steam/isteamnetworkingsockets.h>
#include <steam/isteamnetworkingutils.h>

namespace idk
{
    template<typename T>
    inline void ConnectionManager::SendMessage(T&& message, GameChannel delivery_mode)
    {
        WriteStream stream{ message_buffer, message_buffer_size };

        using Stream = WriteStream;
        serialize_int(stream, MessageID<std::decay_t<T>>, 0, MessageCount);
        if (message.SerializeInternal(stream))
            SendMessage(CreateMessage(stream.Flush(), delivery_mode));
    }

    template<typename T, typename Func, typename>
    inline void ConnectionManager::CreateAndSendMessage(GameChannel channel, Func&& func)
    {
        T msg;
        func(msg);
        SendMessage(msg, channel);
    }

    template<typename Manager>
    inline Manager* ConnectionManager::GetManager()
    {
        return static_cast<Manager*>(GetManager(Manager::type));
    }
}