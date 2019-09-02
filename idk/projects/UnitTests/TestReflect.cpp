#include "pch.h"
#include <reflect/reflect.h>
#include <util/enum.h>
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
	double blaze_it = 420.0;
	hash_table<Guid, string> hashtable;
};
REFLECT_BEGIN(reflect_this, "reflect_this")
REFLECT_VARS(vec, f, container, blaze_it, hashtable)
REFLECT_END()

TEST(Reflect, TestReflectConstexpr)
{
	auto t = reflect::get_type("vec3");
	EXPECT_EQ(t.hash(), reflect::typehash<vec3>());
	EXPECT_EQ(t.name(), "vec3");

	t = reflect::get_type<span<int>>();
	EXPECT_EQ(t.hash(), reflect::typehash<span<int>>());
	EXPECT_STREQ(string{ t.name() }.c_str(), "idk::span<int>");

	EXPECT_STREQ(string{ reflect::fully_qualified_nameof<float>() }.c_str(), "float");
	EXPECT_STREQ(string{ reflect::fully_qualified_nameof<vec4>() }.c_str(), "idk::tvec<float,4>");
	EXPECT_STREQ(string{ reflect::fully_qualified_nameof<reflect_this>() }.c_str(), "reflect_this");
	EXPECT_STREQ(string{ reflect::fully_qualified_nameof<array>() }.c_str(), "idk::array");

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
	obj.hashtable.emplace(Guid::Make(), "test0");
	obj.hashtable.emplace(Guid::Make(), "test1");

	int counter = 0;
	reflect::dynamic dyn{ obj };
	dyn.visit([&](auto&& name, auto&& mem, int) {
		using T = std::decay_t<decltype(mem)>;
		auto val = mem;
		++counter;
		return false; // false should stop recursive
	});

	EXPECT_EQ(counter, 5);


	std::vector<reflect::dynamic> visited_keys;
	std::vector<reflect::dynamic> visited_values;
	std::vector<int> depth_changes;
	dyn.visit([&](auto&& key, auto&& val, int depth_change) {
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
	EXPECT_STREQ(visited_keys[11].get<const char*>(), "blaze_it");
	EXPECT_STREQ(visited_keys[12].get<const char*>(), "hashtable");
	EXPECT_EQ(visited_keys[13].get<Guid>(), obj.hashtable.begin()->first);
	EXPECT_EQ(visited_keys[14].get<Guid>(), (++obj.hashtable.begin())->first);

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
	EXPECT_EQ(visited_values[11].get<double>(), 420.0);
	//EXPECT_EQ(visited_values[12], );
	EXPECT_STREQ(visited_values[13].get<string>().c_str(), obj.hashtable.begin()->second.c_str());
	EXPECT_STREQ(visited_values[14].get<string>().c_str(), (++obj.hashtable.begin())->second.c_str());

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
	EXPECT_EQ(depth_changes[11], -1);
	EXPECT_EQ(depth_changes[12], 0);
	EXPECT_EQ(depth_changes[13], 1);
	EXPECT_EQ(depth_changes[14], 0);


	dyn.visit([&](auto&& key, auto&& val, int) {
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

TEST(Reflect, TestReflectUniContainer)
{
	{
		array<int, 4> arr{ 1, 2, 3, 4 };
		auto container = reflect::dynamic{ arr }.to_container();

		EXPECT_TRUE(container.type.is_template<std::array>());

		std::vector<reflect::dynamic> values;
		for (auto& elem : container)
		{
			values.push_back(elem);
		}

		EXPECT_EQ(values[0].get<int>(), 1);
		EXPECT_EQ(values[1].get<int>(), 2);
		EXPECT_EQ(values[2].get<int>(), 3);
		EXPECT_EQ(values[3].get<int>(), 4);
		EXPECT_EQ(container.size(), 4);
		EXPECT_THROW(container.add(5), const char*);
		EXPECT_THROW(container.clear(), const char*);
	}

	{
		vector<float> vec{ 1.0f, 2.0f, 3.0f, 4.0f };
		reflect::uni_container container{ vec };

		EXPECT_TRUE(container.type.is_template<std::vector>());

		container.add(5.0f);

		std::vector<reflect::dynamic> values;
		for (auto& elem : container)
		{
			values.push_back(elem);
		}

		EXPECT_EQ(values[0].get<float>(), 1.0f);
		EXPECT_EQ(values[1].get<float>(), 2.0f);
		EXPECT_EQ(values[2].get<float>(), 3.0f);
		EXPECT_EQ(values[3].get<float>(), 4.0f);
		EXPECT_EQ(values[4].get<float>(), 5.0f);
		EXPECT_EQ(container.size(), 5);

		container.clear();
		EXPECT_EQ(container.size(), 0);
	}

	{
		hash_table<char, double> map{ { 'a', 1.0 }, { 'b', 2.0 }, { 'c', 3.0 }, { 'd', 4.0 } };
		reflect::uni_container container{ map };

		EXPECT_TRUE(container.type.is_template<std::unordered_map>());

		container.add(std::pair<const char, double>{ 'e', 5.0f });

		std::vector<reflect::dynamic> values;
		for (auto& elem : container)
		{
			values.push_back(elem);
		}

		EXPECT_EQ(values[4].unpack()[0].get<char>(), 'e');
		EXPECT_EQ(values[4].unpack()[1].get<double>(), 5.0);

		EXPECT_EQ(container.size(), 5);

		container.clear();
		EXPECT_EQ(container.size(), 0);
	}
}

namespace idk
{
	ENUM(testenum, char, IVAN = 5, IS, A, WEEB)
}
REFLECT_ENUM(idk::testenum, "testenum")

TEST(Reflect, TestReflectEnum)
{

	EXPECT_TRUE(is_macro_enum<testenum>::value);
	EXPECT_FALSE(is_macro_enum<vec3>::value);

	auto t = reflect::get_type<testenum>().as_enum_type();

	std::vector<string_view> names;
	std::vector<int64_t> values;
	for (auto [name, value] : t)
	{
		names.push_back(name);
		values.push_back(value);
	}

	EXPECT_EQ(names[0], "IVAN");
	EXPECT_EQ(names[1], "IS");
	EXPECT_EQ(names[2], "A");
	EXPECT_EQ(names[3], "WEEB");
	EXPECT_EQ(values[0], 5);
	EXPECT_EQ(values[1], 6);
	EXPECT_EQ(values[2], 7);
	EXPECT_EQ(values[3], 8);

	testenum x = testenum::IVAN;
	EXPECT_EQ(x, 5);

	auto ivan = t.from_string("WEEB");
	EXPECT_EQ(ivan, testenum(testenum::WEEB));
}

struct unknowntest : reflect_this
{
	testenum t = testenum::A;
};
REFLECT_BEGIN(unknowntest, "unknowntest")
REFLECT_PARENT(reflect_this)
REFLECT_VARS(t)			    
REFLECT_END()

TEST(Reflect, TestParentAndUnknownVisit)
{
	unknowntest v;
	int count = 0;
	reflect::visit(v, [&](auto&& key, auto&& val, int depth_change)
	{
		++count;
	});

	// reflect_this, reflect_this members, vec4 members, testenum, testenum value
	EXPECT_EQ(count, 1 + reflect::get_type<reflect_this>().count() + 4 + 1 + 1);
}

struct varianttest
{
	variant<bool, float, int, vec2, vec3, vec4, mat3, mat4> uniform;
};
REFLECT_BEGIN(varianttest, "varianttest")
REFLECT_VARS(uniform)
REFLECT_END()
TEST(Reflect, TestVisitVariant)
{
	varianttest test{ mat4{ 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16 } };

	std::vector<reflect::dynamic> visited_keys;
	std::vector<reflect::dynamic> visited_values;

	reflect::visit(test, [&](auto&& key, auto&& val, int depth_change)
	{
		visited_keys.emplace_back(std::forward<decltype(key)>(key));
		visited_values.emplace_back(val);
	});

	EXPECT_STREQ(visited_keys[0].get<const char*>(), "uniform");
	EXPECT_TRUE(visited_keys[1].get<reflect::type>().is<mat4>());
	EXPECT_EQ(visited_values[1].get<mat4>(), std::get<7>(test.uniform));
}