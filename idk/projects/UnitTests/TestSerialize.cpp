#include "pch.h"
#include <serialize/serialize.h>
#include <reflect/ReflectRegistration.h>
#include <res/Guid.h>
#include <scene/SceneFactory.h>

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
	EXPECT_TRUE(is_basic_serializable<float>::value);
	EXPECT_FALSE(is_basic_serializable<vec3>::value);
	EXPECT_TRUE(is_basic_serializable<Guid>::value);

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
	EXPECT_EQ(t0->position, 4.f * vec3(1.0f, 2.0f, 3.0f));
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