#include "pch.h"
#include <reflect/reflect.h>

TEST(Reflect, TestReflect)
{
	using namespace idk;

	auto t = reflect::get_type("vec3");
	EXPECT_STREQ(t.name().data(), "vec3");
	
	auto v = t.create();
	EXPECT_TRUE(v.valid());
	EXPECT_TRUE(v.is<vec3>());
	EXPECT_FALSE(v.is<vec2>());
	EXPECT_EQ(v.get<vec3>().length(), 0);

	auto v2 = t.create(1.0f, 1.0f, 1.0f);
	EXPECT_TRUE(v.valid());
	EXPECT_TRUE(v.get<vec3>().x == v.get<vec3>().y && v.get<vec3>().y == v.get<vec3>().z);
	auto v2v = v2.get<vec3>();
	auto v2l = v2.get<vec3>().length();

	v = v2;
	EXPECT_TRUE(v.valid());
	EXPECT_EQ(v.get<vec3>().length(), v2l);

	v2.get<vec3>() *= 2;
	EXPECT_NE(v.get<vec3>(), v2.get<vec3>());
}