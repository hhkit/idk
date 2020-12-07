#include "pch.h"
#include <idk.h>
#include <network/MessageStream.h>

struct HelloMessage
{
    bool a = true;
    int b = 69;
    float c = 420.0f;
    string d = "testing1234";
};

template<typename Stream>
bool test_message_serialize(Stream& stream, HelloMessage& msg)
{
    // 1 byte
    serialize_bool(stream, msg.a);
    serialize_int(stream, msg.b, 0, 127);

    // 4 bytes
    serialize_float(stream, msg.c);

    // 1 byte (6 bits, but aligned to 1 byte before writing byte stream)
    serialize_vector_count(stream, msg.d, 63);

    // 11 bytes
    serialize_byte_stream(stream, msg.d);

    return true;
}

TEST(MessageStream, TestMessageStream)
{
    using namespace idk;

    std::byte buffer[512];
    HelloMessage msg;

    WriteStream write{ buffer, 512 };
    EXPECT_TRUE(test_message_serialize(write, msg));
    EXPECT_EQ(write.Flush(), 17);

    // roundtrip
    ReadStream read{ buffer, 512 };
    EXPECT_TRUE(test_message_serialize(read, msg));

    HelloMessage msg2;
    EXPECT_EQ(msg.a, msg2.a);
    EXPECT_EQ(msg.b, msg2.b);
    EXPECT_EQ(msg.c, msg2.c);
    EXPECT_EQ(msg.d, msg2.d);
}