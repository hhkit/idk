#include "pch.h"
#include <serialize/serialize.h>
#include <reflect/ReflectRegistration.h>
#include <res/Guid.h>
#include <scene/SceneFactory.h>
#include <util/enum.h>

using namespace idk;

ENUM(testserialize_enum, int, PI, TAU)
//REFLECT_ENUM(testserialize_enum, "testserialize_enum")

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
	EXPECT_TRUE(is_basic_serializable<float>::value);
	EXPECT_FALSE(is_basic_serializable<vec3>::value);
	EXPECT_TRUE(is_basic_serializable<Guid>::value);
	EXPECT_TRUE(is_basic_serializable<testserialize_enum>::value);

	testserialize_enum e = testserialize_enum::PI;
	EXPECT_STREQ(serialize_text(e).c_str(), "PI");

	vec3 v{ 1.0f, 2.0f, 3.0f };
	auto str = serialize_text(v);
	std::cout << str;
	EXPECT_STREQ(str.c_str(), "{\n  \"x\": 1.0,\n  \"y\": 2.0,\n  \"z\": 3.0\n}");

	serialize_this obj = {
		Guid{"e82bf459-faca-4c70-a8e9-dd35597575ef"},
		vec4{5.0f, 6.0f, 7.0f, 8.0f}
	};
	auto x = "{\n  \"f\": 69,\n  "
		"\"guid\": \"e82bf459-faca-4c70-a8e9-dd35597575ef\",\n  "
		"\"vec\": {\n    \"w\": 8.0,\n    \"x\": 5.0,\n    \"y\": 6.0,\n    \"z\": 7.0\n  }\n}";
	str = serialize_text(obj);
	std::cout << str;
	EXPECT_STREQ(str.c_str(), x);

	// roundtrip
	auto obj2 = parse_text<serialize_this>(str);
	EXPECT_EQ(obj.f, obj2.f);
	EXPECT_EQ(obj.guid, obj2.guid);
	EXPECT_EQ(obj.vec, obj2.vec);
}

struct serialize_this_bs
{
	int start = 0;
	vector<string> string_vec;
	float mid = 1.0f;
	hash_table<Guid, string> hashtable;
	char end = '2';
};
REFLECT_BEGIN(serialize_this_bs, "serialize_this_bs")
REFLECT_VARS(start, string_vec, mid, hashtable, end)
REFLECT_END()

TEST(Serialize, TestSerializeComplex)
{
	serialize_this_bs bs;
	bs.string_vec = { "ivan", "is", "a", "weeb" };
	bs.hashtable.emplace(Guid::Make(), "test0");
	bs.hashtable.emplace(Guid::Make(), "test1");

	auto str = serialize_text(bs);
	std::cout << str;
	
	// roundtrip
	serialize_this_bs bs2 = parse_text<serialize_this_bs>(str);
	EXPECT_EQ(bs2.start, bs.start);
	EXPECT_EQ(bs2.mid, bs.mid);
	EXPECT_EQ(bs2.end, bs.end);
	EXPECT_EQ(bs2.string_vec[0], bs.string_vec[0]);
	EXPECT_EQ(bs2.string_vec[1], bs.string_vec[1]);
	EXPECT_EQ(bs2.string_vec[2], bs.string_vec[2]);
	EXPECT_EQ(bs2.string_vec[3], bs.string_vec[3]);
	EXPECT_EQ(bs2.hashtable[bs.hashtable.begin()->first], bs.hashtable.begin()->second);
	EXPECT_EQ(bs2.hashtable[(++bs.hashtable.begin())->first], (++bs.hashtable.begin())->second);
}

static string serialized_scene_0 = "";
static uint64_t transform_0_id = 0;
static uint64_t transform_1_id = 0;
static uint64_t parent_1_id = 0;

TEST(Serialize, TestSerializeScene)
{
	GameState gs;
	SceneFactory sf;
	auto scene = sf.Create();

	auto o0 = scene->CreateGameObject();
	auto t0 = o0->GetComponent<Transform>();
	t0->position = vec3{ 1.0f, 2.0f, 3.0f };
	t0->scale = vec3{ 4.0f };
	t0->rotation = quat{ 5.0f, 6.0f, 7.0f, 8.0f };
	transform_0_id = t0.id;

	auto o1 = scene->CreateGameObject();
	auto t1 = o1->GetComponent<Transform>();
	t1->position = vec3{ 9.0f, 10.0f, 11.0f };
	t1->scale = vec3{ 12.0f };
	t1->rotation = quat{ 13.0f, 14.0f, 15.0f, 16.0f };
	transform_1_id = t1.id;
	auto p1 = o1->AddComponent<Parent>();
	p1->parent = o0;

	serialized_scene_0 = serialize_text(*scene);
	std::cout << serialized_scene_0;
}

TEST(Serialize, TestParseScene)
{
	GameState gs;
	SceneFactory sf;
	auto scene = sf.Create();

	parse_text(serialized_scene_0, *scene);

	auto& o0 = *scene->begin();
	auto t0 = o0.GetComponent<Transform>();
	EXPECT_EQ(t0->position, vec3(1.0f, 2.0f, 3.0f));
	EXPECT_EQ(t0->scale, vec3{ 4.0f });
	EXPECT_EQ(t0->rotation, quat(5.0f, 6.0f, 7.0f, 8.0f));
	EXPECT_EQ(t0.id, transform_0_id);

	auto& o1 = *++scene->begin();
	auto t1 = o1.GetComponent<Transform>();
	EXPECT_EQ(t1->position, vec3(9.0f, 10.0f, 11.0f));
	EXPECT_EQ(t1->scale, vec3{ 12.0f });
	EXPECT_EQ(t1->rotation, quat(13.0f, 14.0f, 15.0f, 16.0f));
	EXPECT_EQ(t1.id, transform_1_id);
	auto p1 = o1.GetComponent<Parent>();
	EXPECT_EQ(p1->parent.id, o0.GetHandle().id);
}