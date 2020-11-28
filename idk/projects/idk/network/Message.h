#pragma once
#include <network/MessageStream.h>
#include <steam/steamnetworkingtypes.h>

namespace idk
{
    using NetworkMessageTuple = std::tuple<
        class TestMessage
        , class EventDataBlockFrameNumber
        , class EventDataBlockBufferedEvents
        , class EventInstantiatePrefabMessage
        , class EventLoadLevelMessage
        , class EventTransferOwnershipMessage
        , class EventInvokeRPCMessage
        , class EventDestroyObjectMessage
        , class GhostMessage
        , class GhostAcknowledgementMessage
        , class MoveClientMessage
        , class ControlObjectMessage
    >;

    static constexpr uint8_t MessageCount = std::tuple_size_v<NetworkMessageTuple>;
    template<typename T>
    static constexpr uint8_t MessageID = index_in_tuple_v<T, NetworkMessageTuple>;

    class Message
    {
    public:
        virtual bool SerializeInternal(WriteStream& stream) = 0;
        virtual bool SerializeInternal(ReadStream& stream) = 0;
        virtual ~Message() {}
    };
}

#define NETWORK_MESSAGE_VIRTUAL_SERIALIZE_FUNCTIONS()								   \
    bool SerializeInternal(WriteStream& stream) override { return Serialize(stream); } \
    bool SerializeInternal(ReadStream& stream) override { return Serialize(stream); }
