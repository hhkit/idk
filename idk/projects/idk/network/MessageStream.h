#pragma once
#include <idk.h>

namespace idk
{
    namespace detail
    {
        inline consteval int bits_required(int32_t min, int32_t max)
        {
            uint32_t val = max - min;
            int ret = 0;
            while (val)
            {
                val >>= 1;
                ++ret;
            }
            return ret;
        }
    }

    class MessageStream
    {
    };

    class WriteStream : MessageStream
    {
    public:
        enum { IsWriting = 1 };
        enum { IsReading = 0 };

        WriteStream(std::byte* buffer, uint32_t size);

        void SerializeBits(uint32_t val, int bits);
        void SerializeUInt32(uint32_t val);
        void SerializeUInt64(uint64_t val);
        void SerializeFloat(float val);
        void SerializeBytes(const std::byte* data, int bytes);

        // Flush scratch bits and return total bytes written.
        int Flush();

    private:
        uint32_t* buffer_;
        uint32_t size_ = 0;
        uint32_t offset_ = 0;
        uint64_t scratch_ = 0;
        int scratch_bits_ = 0;

        void WriteBits(uint32_t value, int bits);
        void WriteAlign();
        void WriteBytes(const std::byte* data, int bytes);
    };

    class ReadStream : MessageStream
    {
    public:
        enum { IsWriting = 0 };
        enum { IsReading = 1 };

        ReadStream(const std::byte* buffer, uint32_t size);

        void SerializeBits(uint32_t& val, int bits);
        void SerializeUInt32(uint32_t& val);
        void SerializeUInt64(uint64_t& val);
        void SerializeFloat(float& val);
        void SerializeBytes(std::byte* data, int bytes);

    private:
        const uint32_t* buffer_;
        uint32_t size_ = 0;
        uint32_t offset_ = 0;
        uint64_t scratch_ = 0;
        int scratch_bits_ = 0;

        uint32_t ReadBits(int bits);
        void ReadAlign();
        void ReadBytes(std::byte* data, int bytes);
    };

}

#define serialize_int(stream, value, min, max)                        \
{                                                                     \
    uint32_t val;                                                     \
    if constexpr (Stream::IsWriting)                                  \
        val = value - min;                                            \
    static constexpr int bits = idk::detail::bits_required(min, max); \
    stream.SerializeBits(val, bits);                                  \
    if constexpr (Stream::IsReading)                                  \
        value = val + min;                                            \
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
    uint32_t val;                     \
    if constexpr (Stream::IsWriting)  \
        val = value ? 1 : 0;          \
    stream.SerializeBits(val, 1);     \
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

#define serialize_bytes(stream, data, bytes)        \
{                                                   \
    stream.SerializeBytes((std::byte*)data, bytes); \
}

#define serialize_vector_count(stream, vec, max_size)   \
{                                                       \
    uint32_t count = static_cast<uint32_t>(vec.size()); \
    serialize_int(stream, count, 0, max_size);          \
    vec.resize(count);                                  \
}

#define serialize_byte_stream(stream, vec) \
    serialize_bytes(stream, (std::byte*) vec.data(), static_cast<int>(vec.size()));

#define serialize_dataonly_struct(stream, obj) \
    serialize_bytes(stream, (std::byte*) &obj, sizeof(obj))

#define serialize_vec(stream, vec)      \
{                                       \
    for (auto& coord : vec)             \
        serialize_float(stream, coord); \
}