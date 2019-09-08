#include "pch.h"
#include <scene/SceneFactory.h>
#include <prefab/PrefabUtility.h>
#include <prefab/PrefabFactory.h>
#include <IncludeComponents.h>
#include <IncludeSystems.h>
#include <IncludeResources.h>
#include <res/ForwardingExtensionLoader.h>

using namespace idk;

TEST(Prefab, TestPrefabSave)
{
    Core core;
    FileSystem& fs = core.GetSystem<FileSystem>();
    fs.Init();

    SceneFactory sf;
    auto scene = sf.GenerateDefaultResource();

    auto go = scene->CreateGameObject();
    
    go->GetComponent<Name>()->name = "stoopidguy";

    auto t0 = go->GetComponent<Transform>();
    t0->position = vec3{ 1.0f, 2.0f, 3.0f };
    t0->scale = vec3{ 4.0f };
    t0->rotation = quat{ 5.0f, 6.0f, 7.0f, 8.0f };

    auto child = scene->CreateGameObject();
    auto t1 = child->GetComponent<Transform>();
    t1->position = vec3{ 9.0f, 10.0f, 11.0f };
    t1->scale = vec3{ 12.0f };
    t1->rotation = quat{ 13.0f, 14.0f, 15.0f, 16.0f };
	child->Transform()->parent = go;

    auto exe_dir = std::string{ fs.GetExeDir() };
    fs.Mount(exe_dir + "/assets", "/assets");
    fs.SetAssetDir(exe_dir + "/assets");

    fs.Open("/assets/prefabs/testprefab.idp", FS_PERMISSIONS::WRITE);
    auto save_path = fs.GetFile("/assets/prefabs/testprefab.idp");

    EXPECT_NO_THROW(PrefabUtility::Save(go, save_path));
}

TEST(Prefab, TestPrefabInstantiate)
{
    Core core;
    FileSystem& fs = core.GetSystem<FileSystem>();
    fs.Init();

    SceneFactory sf;
    auto scene = sf.GenerateDefaultResource();

    core.GetResourceManager().Init();

    auto& pf = core.GetResourceManager().RegisterFactory<PrefabFactory>();
    core.GetResourceManager().RegisterExtensionLoader<ForwardingExtensionLoader<Prefab>>(".idp");

    auto& prefab = core.GetResourceManager().LoadFile(fs.GetFile("/assets/prefabs/testprefab.idp"))[0].As<Prefab>();

    {
        auto& data = prefab->data;

        auto t0 = data[0].components[0].get<Transform>();
        EXPECT_EQ(t0.position, vec3(1.0f, 2.0f, 3.0f));
        EXPECT_EQ(t0.scale, vec3{ 4.0f });
        EXPECT_EQ(t0.rotation, quat(5.0f, 6.0f, 7.0f, 8.0f));
        EXPECT_EQ(data[0].parent_index, -1);
        EXPECT_EQ(data[0].components.size(), 2);
        EXPECT_EQ(data[0].components[1].get<Name>().name, "stoopidguy");

        auto t1 = data[1].components[0].get<Transform>();
        EXPECT_EQ(t1.position, vec3(9.0f, 10.0f, 11.0f));
        EXPECT_EQ(t1.scale, vec3{ 12.0f });
        EXPECT_EQ(t1.rotation, quat(13.0f, 14.0f, 15.0f, 16.0f));
        EXPECT_EQ(data[1].parent_index, 0);
        EXPECT_EQ(data[1].components.size(), 2);
    }

    auto go = PrefabUtility::Instantiate(prefab, *scene);
    {
        auto& o0 = *scene->begin();
        auto t0 = o0.GetComponent<Transform>();
        EXPECT_EQ(t0->position, vec3(1.0f, 2.0f, 3.0f));
        EXPECT_EQ(t0->scale, vec3{ 4.0f });
        EXPECT_EQ(t0->rotation, quat(5.0f, 6.0f, 7.0f, 8.0f));
        EXPECT_EQ(o0.GetComponent<Name>()->name, "stoopidguy");

        auto& o1 = *++scene->begin();
        auto t1 = o1.GetComponent<Transform>();
        EXPECT_EQ(t1->position, vec3(9.0f, 10.0f, 11.0f));
        EXPECT_EQ(t1->scale, vec3{ 12.0f });
        EXPECT_EQ(t1->rotation, quat(13.0f, 14.0f, 15.0f, 16.0f));
        EXPECT_EQ(o1.Transform()->parent.id, o0.GetHandle().id);
    }
}

TEST(Prefab, TestPrefabRevert)
{
    Core core;
    FileSystem& fs = core.GetSystem<FileSystem>();
    fs.Init();

    SceneFactory sf;
    auto scene = sf.GenerateDefaultResource();

    core.GetResourceManager().Init();

    auto& pf = core.GetResourceManager().RegisterFactory<PrefabFactory>();
    core.GetResourceManager().RegisterExtensionLoader<ForwardingExtensionLoader<Prefab>>(".idp");

    auto& prefab = core.GetResourceManager().LoadFile(fs.GetFile("/assets/prefabs/testprefab.idp"))[0].As<Prefab>();
    auto go = PrefabUtility::Instantiate(prefab, *scene);

    vec3 ori_scale = go->GetComponent<Transform>()->scale;

    (*go->GetComponents()[0]).get_property("position").value = vec3(69.0f, 69.0f, 69.0f);
    (*go->GetComponents()[0]).get_property("scale").value = vec3(69.0f, 69.0f, 69.0f);
    (*go->GetComponents()[1]).get_property("name").value = string("changed_name");
    PrefabUtility::RecordPrefabInstanceChange(go, go->GetComponents()[0], "position");
    PrefabUtility::RecordPrefabInstanceChange(go, go->GetComponents()[0], "scale");
    PrefabUtility::RecordPrefabInstanceChange(go, go->GetComponents()[1], "name");
    // add some completely bullshit property overrides
    go->GetComponent<PrefabInstance>()->overrides.emplace_back(PropertyOverride{ 0, "Transform", "scrimbux" });
    go->GetComponent<PrefabInstance>()->overrides.emplace_back(PropertyOverride{ 0, "MissingNo", "scrimbux" });
    go->GetComponent<PrefabInstance>()->overrides.emplace_back(PropertyOverride{ 1, "Transform", "scrimbux" });

    auto t0 = go->GetComponent<Transform>();
    EXPECT_EQ(t0->position, vec3(69.0f, 69.0f, 69.0f));
    EXPECT_EQ(t0->scale, vec3(69.0f, 69.0f, 69.0f));
    EXPECT_EQ(go->GetComponent<Name>()->name, "changed_name");

    PrefabUtility::RevertPrefabInstance(go);
    EXPECT_EQ(t0->position, vec3(69.0f, 69.0f, 69.0f)) << "Position and rotation should not be reverted.";
    EXPECT_EQ(t0->scale, ori_scale);
    EXPECT_EQ(go->GetComponent<Name>()->name, "changed_name") << "Name should not be reverted.";
}

TEST(Prefab, TestPrefabPropagate)
{
    Core core;
    FileSystem& fs = core.GetSystem<FileSystem>();
    fs.Init();

    SceneFactory sf;
    auto scene = sf.GenerateDefaultResource();

    core.GetResourceManager().Init();

    auto& pf = core.GetResourceManager().RegisterFactory<PrefabFactory>();
    core.GetResourceManager().RegisterExtensionLoader<ForwardingExtensionLoader<Prefab>>(".idp");

    auto& prefab = core.GetResourceManager().LoadFile(fs.GetFile("/assets/prefabs/testprefab.idp"))[0].As<Prefab>();
    auto go0 = PrefabUtility::Instantiate(prefab, *scene);
    auto go1 = PrefabUtility::Instantiate(prefab, *scene);
    auto go2 = PrefabUtility::Instantiate(prefab, *scene);
    auto go3 = PrefabUtility::Instantiate(prefab, *scene);

    prefab->data[0].components[0].get<Transform>().position = vec3(69.0f, 69.0f, 69.0f);
    prefab->data[0].components[0].get<Transform>().scale = vec3(69.0f, 69.0f, 69.0f);
    prefab->data[0].components[1].get<Name>().name = string("changed_name");

    vec3 ori_pos = go0->GetComponent<Transform>()->position;

    PrefabUtility::PropagatePrefabChangesToInstances(prefab);

    EXPECT_EQ(go0->GetComponent<Transform>()->position, ori_pos) << "Position and rotation should not be propagated.";
    EXPECT_EQ(go0->GetComponent<Transform>()->scale, vec3(69.0f, 69.0f, 69.0f));
    EXPECT_EQ(go0->GetComponent<Name>()->name, "stoopidguy") << "Name should not be propagated.";

    EXPECT_EQ(go1->GetComponent<Transform>()->position, ori_pos) << "Position and rotation should not be propagated.";
    EXPECT_EQ(go1->GetComponent<Transform>()->scale, vec3(69.0f, 69.0f, 69.0f));
    EXPECT_EQ(go1->GetComponent<Name>()->name, "stoopidguy") << "Name should not be propagated.";

    EXPECT_EQ(go2->GetComponent<Transform>()->position, ori_pos) << "Position and rotation should not be propagated.";
    EXPECT_EQ(go2->GetComponent<Transform>()->scale, vec3(69.0f, 69.0f, 69.0f));
    EXPECT_EQ(go2->GetComponent<Name>()->name, "stoopidguy") << "Name should not be propagated.";

    EXPECT_EQ(go3->GetComponent<Transform>()->position, ori_pos) << "Position and rotation should not be propagated.";
    EXPECT_EQ(go3->GetComponent<Transform>()->scale, vec3(69.0f, 69.0f, 69.0f));
    EXPECT_EQ(go3->GetComponent<Name>()->name, "stoopidguy") << "Name should not be propagated.";
}