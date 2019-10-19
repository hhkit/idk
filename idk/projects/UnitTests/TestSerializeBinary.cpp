#include "pch.h"
#include <serialize/binary.h>
#include <ReflectRegistration.h>

using namespace idk;

struct serialize_this
{
    Guid guid;
    vec4 vec;
    int f = 69;
};
REFLECT_BEGIN(serialize_this, "serialize_this")
REFLECT_VARS(guid, vec, f)
REFLECT_END()

TEST(Serialize, TestSerializeBinaryBasic)
{
    serialize_this obj = {
        Guid{"e82bf459-faca-4c70-a8e9-dd35597575ef"},
        vec4{5.0f, 6.0f, 7.0f, 8.0f}
    };
    string str = serialize_binary(obj);
    serialize_this obj2;

    EXPECT_EQ(parse_binary(str, obj2), parse_error::none);

    EXPECT_EQ(obj.guid, obj2.guid);
    EXPECT_EQ(obj.vec, obj2.vec);
    EXPECT_EQ(obj.f, obj2.f);
}

TEST(Serialize, TestSerializeBinaryVectorOfDynamics)
{
    vector<reflect::dynamic> v;
    v.push_back(5);
    v.push_back({});
    v.push_back(UniformInstance{ "test", 420.0f });
    string str = serialize_binary(v);
    v.pop_back();

    EXPECT_EQ(parse_binary(str, v), parse_error::none);

    EXPECT_EQ(v.size(), 3);
    EXPECT_TRUE(v[0].valid());
    EXPECT_TRUE(!v[1].valid());
    EXPECT_TRUE(v[2].valid());

    EXPECT_EQ(v[0].get<int>(), 5);
    EXPECT_EQ(v[2].get<UniformInstance>().name, "test");
    EXPECT_EQ(v[2].get<UniformInstance>().value, UniformInstanceValue(420.0f));
}

ENUM(testenumber, int, One = 1, Two, Three)
REFLECT_ENUM(testenumber, "testenumber")
TEST(Serialize, TestSerializeBinaryHashTable)
{
    hash_table<string, testenumber> ht;
    ht.emplace("one", testenumber::One);
    ht.emplace("two", testenumber::Two);
    ht.emplace("three", testenumber::Three);
    string str = serialize_binary(ht);
    ht.erase("three");

    EXPECT_EQ(parse_binary(str, ht), parse_error::none);

    EXPECT_EQ(ht.size(), 3);
    EXPECT_EQ(ht["one"], testenumber::One);
    EXPECT_EQ(ht["two"], testenumber::Two);
    EXPECT_EQ(ht["three"], testenumber::Three);
}