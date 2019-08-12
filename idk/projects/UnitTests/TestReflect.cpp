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
};
REFLECT_BEGIN(reflect_this, "reflect_this")
	REFLECT_VAR(vec)
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


	int counter = 0;
	reflect::dynamic dyn{ obj };
	dyn.visit([&](const char* name, auto&& mem) {
		using T = std::decay_t<decltype(mem)>;
		auto val = mem;
		++counter;
		return false; // false should stop recursive
	});

	EXPECT_EQ(counter, 1);


	std::vector<const char*> visited_names;
	std::vector<reflect::dynamic> visited_values;
	std::vector<reflect::type> visited_types;
	dyn.visit([&](const char* name, auto&& mem) {
		using T = std::decay_t<decltype(mem)>;

		visited_names.push_back(name);
		visited_values.emplace_back(mem);
		visited_types.emplace_back(reflect::get_type<T>());
	});

	EXPECT_STREQ(visited_names[0], "vec");
	EXPECT_STREQ(visited_names[1], "x");
	EXPECT_STREQ(visited_names[2], "y");
	EXPECT_STREQ(visited_names[3], "z");
	EXPECT_STREQ(visited_names[4], "w");

	EXPECT_EQ(visited_values[0].get<vec4>(), vec4(1.0f, 2.0f, 3.0f, 4.0f));
	EXPECT_EQ(visited_values[1].get<float>(), 1.0f);
	EXPECT_EQ(visited_values[2].get<float>(), 2.0f);
	EXPECT_EQ(visited_values[3].get<float>(), 3.0f);
	EXPECT_EQ(visited_values[4].get<float>(), 4.0f);

	EXPECT_STREQ(visited_types[0].name().data(), "vec4");
	EXPECT_EQ(visited_types[1].hash(), reflect::typehash<float>());
	EXPECT_EQ(visited_types[2].hash(), reflect::typehash<float>());
	EXPECT_EQ(visited_types[3].hash(), reflect::typehash<float>());
	EXPECT_EQ(visited_types[4].hash(), reflect::typehash<float>());


	dyn.visit([&](const char* name, auto&& mem) {
		using T = std::decay_t<decltype(mem)>;

		if constexpr (std::is_same_v<T, float>)
			mem *= 2.0f;
	});

	EXPECT_EQ(obj.vec, vec4(2.0f, 4.0f, 6.0f, 8.0f));
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