#include "pch.h"
#include <reflect/reflect.h>
#include <reflect/ReflectRegistration.h>

using namespace idk;

TEST(Reflect, TestReflectBasic)
{
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

struct reflect_this
{
	vec4 vec;
	int f = 69;
	vector<string> container;
};
REFLECT_BEGIN(reflect_this, "reflect_this")
REFLECT_VARS(vec, f, container)
REFLECT_END()

TEST(Reflect, TestReflectConstexpr)
{
	auto t = reflect::get_type("vec3");
	EXPECT_EQ(t.hash(), reflect::typehash<vec3>());
	EXPECT_EQ(t.name(), "vec3");

	t = reflect::get_type<span<int>>();
	EXPECT_EQ(t.hash(), reflect::typehash<span<int>>());
	EXPECT_STREQ(string{ t.name() }.c_str(), "idk::span<int>");

	EXPECT_STREQ(string{ reflect::nameof<float>() }.c_str(), "float");
	EXPECT_STREQ(string{ reflect::nameof<vec4>() }.c_str(), "idk::math::vector<float,4>");
	EXPECT_STREQ(string{ reflect::nameof<reflect_this>() }.c_str(), "reflect_this");

	int switch_case = 0;
	switch (t.hash())
	{
		case reflect::typehash<float>(): switch_case = 1; break;
		case reflect::typehash<span<int>>(): switch_case = 2; break;
		default: switch_case = 3; break;
	}
	EXPECT_EQ(switch_case, 2);
}

TEST(Reflect, TestReflectVisit)
{
	reflect_this obj;
	obj.vec = { 1.0f, 2.0f, 3.0f, 4.0f };
	obj.container.push_back("ivan");
	obj.container.push_back("is");
	obj.container.push_back("a");
	obj.container.push_back("weeb");

	int counter = 0;
	reflect::dynamic dyn{ obj };
	dyn.visit([&](auto name, auto&& mem, int) {
		using T = std::decay_t<decltype(mem)>;
		auto val = mem;
		++counter;
		return false; // false should stop recursive
	});

	EXPECT_EQ(counter, 3);


	std::vector<reflect::dynamic> visited_keys;
	std::vector<reflect::dynamic> visited_values;
	std::vector<int> depth_changes;
	dyn.visit([&](auto key, auto&& val, int depth_change) {
		visited_keys.emplace_back(std::forward<decltype(key)>(key));
		visited_values.emplace_back(val);
		depth_changes.push_back(depth_change);
	});

	EXPECT_STREQ(visited_keys[0].get<const char*>(), "vec");
	EXPECT_STREQ(visited_keys[1].get<const char*>(), "x");
	EXPECT_STREQ(visited_keys[2].get<const char*>(), "y");
	EXPECT_STREQ(visited_keys[3].get<const char*>(), "z");
	EXPECT_STREQ(visited_keys[4].get<const char*>(), "w");
	EXPECT_STREQ(visited_keys[5].get<const char*>(), "f");
	EXPECT_STREQ(visited_keys[6].get<const char*>(), "container");
	EXPECT_EQ(visited_keys[7].get<size_t>(), 0);
	EXPECT_EQ(visited_keys[8].get<size_t>(), 1);
	EXPECT_EQ(visited_keys[9].get<size_t>(), 2);
	EXPECT_EQ(visited_keys[10].get<size_t>(), 3);

	EXPECT_EQ(visited_values[0].get<vec4>(), vec4(1.0f, 2.0f, 3.0f, 4.0f));
	EXPECT_EQ(visited_values[1].get<float>(), 1.0f);
	EXPECT_EQ(visited_values[2].get<float>(), 2.0f);
	EXPECT_EQ(visited_values[3].get<float>(), 3.0f);
	EXPECT_EQ(visited_values[4].get<float>(), 4.0f);
	EXPECT_EQ(visited_values[5].get<int>(), 69);
	//EXPECT_EQ(visited_values[6], );
	EXPECT_STREQ(visited_values[7].get<string>().c_str(), "ivan");
	EXPECT_STREQ(visited_values[8].get<string>().c_str(), "is");
	EXPECT_STREQ(visited_values[9].get<string>().c_str(), "a");
	EXPECT_STREQ(visited_values[10].get<string>().c_str(), "weeb");

	EXPECT_EQ(depth_changes[0], 1);
	EXPECT_EQ(depth_changes[1], 1);
	EXPECT_EQ(depth_changes[2], 0);
	EXPECT_EQ(depth_changes[3], 0);
	EXPECT_EQ(depth_changes[4], 0);
	EXPECT_EQ(depth_changes[5], -1);
	EXPECT_EQ(depth_changes[6], 0);
	EXPECT_EQ(depth_changes[7], 1);
	EXPECT_EQ(depth_changes[8], 0);
	EXPECT_EQ(depth_changes[9], 0);
	EXPECT_EQ(depth_changes[10], 0);


	dyn.visit([&](auto key, auto&& val, int) {
		using T = std::decay_t<decltype(val)>;

		if constexpr (std::is_same_v<T, float>)
			val *= 2.0f;
	});

	EXPECT_EQ(obj.vec, vec4(2.0f, 4.0f, 6.0f, 8.0f));
	EXPECT_EQ(obj.f, 69.0f);
}

TEST(Reflect, TestReflectRangeFor)
{
	reflect_this obj;
	obj.vec = { 1.0f, 2.0f, 3.0f, 4.0f };

	reflect::dynamic dyn{ obj };

	std::vector<string_view> names;
	std::vector<reflect::dynamic> values;

	for (auto& [name, val] : dyn.get_property("vec").value)
	{
		names.push_back(name);
		values.emplace_back(val);
	}

	EXPECT_STREQ(names[0].data(), "x");
	EXPECT_STREQ(names[1].data(), "y");
	EXPECT_STREQ(names[2].data(), "z");
	EXPECT_STREQ(names[3].data(), "w");

	EXPECT_EQ(values[0].get<float>(), 1.0f);
	EXPECT_EQ(values[1].get<float>(), 2.0f);
	EXPECT_EQ(values[2].get<float>(), 3.0f);
	EXPECT_EQ(values[3].get<float>(), 4.0f);

	for (auto& [name, val] : dyn.get_property("vec").value)
	{
		val.get<float>() *= 2.0f;
	}

	EXPECT_EQ(obj.vec, vec4(2.0f, 4.0f, 6.0f, 8.0f));
}