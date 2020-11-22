#pragma once
#include <idk.h>
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

    class MessageStream
    {
    };

    class WriteStream : MessageStream
    {
    public:
        enum { IsWriting = 1 };
        enum { IsReading = 0 };

        WriteStream(std::byte* buffer) : buffer_{ buffer } {};
        uint32_t GetSize() { return size_; }

        void SerializeUInt8(uint8_t val) { buffer_[size_++] = static_cast<std::byte>(val); }
        void SerializeInt32(int32_t val) { std::memcpy(buffer_ + size_, &val, sizeof(val)); size_ += sizeof(val); }
        void SerializeUInt32(uint32_t val) { std::memcpy(buffer_ + size_, &val, sizeof(val)); size_ += sizeof(val); }
        void SerializeUInt64(uint64_t val) { std::memcpy(buffer_ + size_, &val, sizeof(val)); size_ += sizeof(val); }
        void SerializeFloat(float val) { std::memcpy(buffer_ + size_, &val, sizeof(val)); size_ += sizeof(val); }
        void SerializeBytes(const uint8_t* data, int bytes) { std::memcpy(buffer_ + size_, data, bytes); size_ += bytes; }

    private:
        std::byte* buffer_;
        uint32_t size_ = 0;
    };

    class ReadStream : MessageStream
    {
    public:
        enum { IsWriting = 0 };
        enum { IsReading = 1 };

        ReadStream(std::byte* buffer, uint32_t size) : buffer_{ buffer }, size_{ size } {};
        void SerializeChar(char& c) { c = static_cast<char>(buffer_[offset_++]); }

        void SerializeUInt8(uint8_t& val) { val = static_cast<uint8_t>(buffer_[offset_++]); }
        void SerializeInt32(int32_t& val) { std::memcpy(&val, buffer_ + offset_, sizeof(val)); offset_ += sizeof(val); }
        void SerializeUInt32(uint32_t& val) { std::memcpy(&val, buffer_ + offset_, sizeof(val)); offset_ += sizeof(val); }
        void SerializeUInt64(uint64_t& val) { std::memcpy(&val, buffer_ + offset_, sizeof(val)); offset_ += sizeof(val); }
        void SerializeFloat(float& val) { std::memcpy(&val, buffer_ + offset_, sizeof(val)); offset_ += sizeof(val); }
        void SerializeBytes(uint8_t* data, int bytes) { std::memcpy(data, buffer_ + offset_, bytes); offset_ += bytes; }

    private:
        std::byte* buffer_;
        uint32_t size_ = 0;
        uint32_t offset_ = 0;
    };

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

// todo: bit packing

#define serialize_int(stream, value, min, max) \
{                                    \
    int32_t val;                     \
    if constexpr (Stream::IsWriting) \
        val = value;                 \
    stream.SerializeInt32(val);      \
    if constexpr (Stream::IsReading) \
        value = val;                 \
}

#define serialize_uint32(stream, value) \
{                                       \
    uint32_t val;                       \
    if constexpr (Stream::IsWriting)    \
        val = value;                    \
    stream.SerializeUInt32(val);        \
    if constexpr (Stream::IsReading)    \
        value = val;                    \
}

#define serialize_uint64(stream, value) \
{                                       \
    uint64_t val;                       \
    if constexpr (Stream::IsWriting)    \
        val = value;                    \
    stream.SerializeUInt64(val);        \
    if constexpr (Stream::IsReading)    \
        value = val;                    \
}

#define serialize_bool(stream, value) \
{                                     \
    uint8_t val;                      \
    if constexpr (Stream::IsWriting)  \
        val = value;                  \
    stream.SerializeUInt8(val);       \
    if constexpr (Stream::IsReading)  \
        value = val;                  \
}

#define serialize_float(stream, value) \
{                                      \
    float val;                         \
    if constexpr (Stream::IsWriting)   \
        val = value;                   \
    stream.SerializeFloat(val);        \
    if constexpr (Stream::IsReading)   \
        value = val;                   \
}

#define serialize_bytes(stream, data, bytes) \
{                                            \
    stream.SerializeBytes(data, bytes);      \
}

#define serialize_vector_count(stream, vec, max_size) \
{                                                     \
    auto count = vec.size();                          \
    serialize_int(stream, count, 0, max_size);        \
    vec.resize(count);                                \
}

#define serialize_byte_stream(stream, vec) \
    serialize_bytes(stream, (uint8_t*) vec.data(), static_cast<int>(vec.size()));

#define serialize_dataonly_struct(stream, obj) \
    serialize_bytes(stream, (uint8_t*) &obj, sizeof(obj))

#define serialize_vec(stream, vec)      \
{                                       \
    for (auto& coord : vec)             \
        serialize_float(stream, coord); \
}