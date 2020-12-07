#include "stdafx.h"
#include "MessageStream.h"

namespace idk
{
    WriteStream::WriteStream(std::byte* buffer, uint32_t size)
        : buffer_{ reinterpret_cast<uint32_t*>(buffer) }, size_{ size }
    {
    }

    void WriteStream::SerializeBits(uint32_t val, int bits)
    {
        WriteBits(val, bits);
    }

    void WriteStream::SerializeUInt32(uint32_t val)
    {
        WriteBits(val, 32);
    }

    void WriteStream::SerializeUInt64(uint64_t val)
    {
        WriteBits(static_cast<uint32_t>(val & 0xFFFFFFFF), 32);
        WriteBits(static_cast<uint32_t>(val >> 32), 32);
    }

    void WriteStream::SerializeFloat(float val) 
    {
        uint32_t word;
        std::memcpy(&word, &val, 4);
        WriteBits(word, 32);
    }

    void WriteStream::SerializeBytes(const std::byte* data, int bytes)
    {
        WriteBytes(data, bytes);
    }

    int WriteStream::Flush()
    {
        int bytes = offset_ * 4 + (scratch_bits_ + 7) / 8;
        buffer_[offset_++] = static_cast<uint32_t>(scratch_ & 0xffffffff);
        scratch_ = 0;
        scratch_bits_ = 0;
        return bytes;
    }

    void WriteStream::WriteBits(uint32_t value, int bits)
    {
        scratch_ |= static_cast<uint64_t>(value) << scratch_bits_;
        scratch_bits_ += bits;
        if (scratch_bits_ >= 32)
        {
            buffer_[offset_++] = static_cast<uint32_t>(scratch_ & 0xffffffff);
            scratch_ >>= 32;
            scratch_bits_ -= 32;
        }
    }

    void WriteStream::WriteAlign()
    {
        scratch_bits_ = (scratch_bits_ + 7) & -8; // align to next byte
        if (scratch_bits_ >= 32)
        {
            buffer_[offset_++] = static_cast<uint32_t>(scratch_ & 0xffffffff);
            scratch_ >>= 32;
            scratch_bits_ -= 32;
        }
    }

    void WriteStream::WriteBytes(const std::byte* data, int bytes)
    {
        WriteAlign();

        int i = 0;

        // fill scratch first
        while (scratch_bits_ && i < bytes)
            WriteBits(static_cast<uint32_t>(data[i++]), 8);
        if (i == bytes)
            return;

        // fill word blocks
        int remainingWords = (bytes - i) / 4;
        std::memcpy(reinterpret_cast<std::byte*>(buffer_ + offset_), data + i, remainingWords * 4);
        offset_ += remainingWords;

        // fill remaining to scratch
        for (i += remainingWords * 4; i < bytes; ++i)
            WriteBits(static_cast<uint32_t>(data[i]), 8);
    }



    ReadStream::ReadStream(const std::byte* buffer, uint32_t size)
        : buffer_{ reinterpret_cast<const uint32_t*>(buffer) }, size_{ size }
    {
    }

    void ReadStream::SerializeBits(uint32_t& val, int bits)
    {
        val = ReadBits(bits);
    }

    void ReadStream::SerializeUInt32(uint32_t& val)
    {
        val = ReadBits(32);
    }

    void ReadStream::SerializeUInt64(uint64_t& val)
    {
        val = ReadBits(32);
        val |= static_cast<uint64_t>(ReadBits(32)) << 32;
    }

    void ReadStream::SerializeFloat(float& val)
    {
        uint32_t word;
        word = ReadBits(32);
        std::memcpy(&val, &word, 4);
    }

    void ReadStream::SerializeBytes(std::byte* data, int bytes)
    {
        ReadBytes(data, bytes);
    }

    uint32_t ReadStream::ReadBits(int bits)
    {
        if (scratch_bits_ < bits)
        {
            scratch_ |= static_cast<uint64_t>(buffer_[offset_++]) << scratch_bits_;
            scratch_bits_ += 32;
        }
        const uint32_t output = scratch_ & ((static_cast<uint64_t>(1) << bits) - 1); // mask first N bits
        scratch_ >>= bits;
        scratch_bits_ -= bits;
        return output;
    }

    void ReadStream::ReadAlign()
    {
        int bits = scratch_bits_ % 8;
        scratch_ >>= bits;
        scratch_bits_ -= bits;
    }

    void ReadStream::ReadBytes(std::byte* data, int bytes)
    {
        ReadAlign();

        int i = 0;

        // read scratch first
        while (scratch_bits_ && i < bytes)
            data[i++] = static_cast<std::byte>(ReadBits(8));
        if (i == bytes)
            return;

        // read word blocks
        int remainingWords = (bytes - i) / 4;
        std::memcpy(data + i, reinterpret_cast<const std::byte*>(buffer_ + offset_), remainingWords * 4);
        offset_ += remainingWords;

        // read remaining to scratch
        for (i += remainingWords * 4; i < bytes; ++i)
            data[i] = static_cast<std::byte>(ReadBits(8));
    }
}