#include "pch.h"
#include <serialize/serialize.h>
#include <reflect/ReflectRegistration.h>
#include <res/Guid.h>

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

TEST(Serialize, TestSerializeBasic)
{
	EXPECT_TRUE(idk::is_basic_serializable<float>::value);
	EXPECT_FALSE(idk::is_basic_serializable<vec3>::value);
	EXPECT_TRUE(idk::is_basic_serializable<Guid>::value);

	vec3 v{ 1.0f, 2.0f, 3.0f };
	auto str = idk::serialize_text(v);
	std::cout << str;
	EXPECT_STREQ(str.c_str(), "{\n  \"x\": 1.0,\n  \"y\": 2.0,\n  \"z\": 3.0\n}");

	serialize_this obj = {
		Guid{"e82bf459-faca-4c70-a8e9-dd35597575ef"},
		vec4{5.0f, 6.0f, 7.0f, 8.0f}
	};
	auto x = "{\n  \"f\": 69,\n  "
		"\"guid\": \"e82bf459-faca-4c70-a8e9-dd35597575ef\",\n  "
		"\"vec\": {\n    \"w\": 8.0,\n    \"x\": 5.0,\n    \"y\": 6.0,\n    \"z\": 7.0\n  }\n}";
	str = idk::serialize_text(obj);
	std::cout << str;
	EXPECT_STREQ(str.c_str(), x);
}