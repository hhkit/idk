#include "pch.h"
#include <scene/SceneFactory.h>
#include <prefab/PrefabUtility.h>
#include <IncludeComponents.h>
#include <IncludeSystems.h>

using namespace idk;

TEST(Prefab, TestPrefab)
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

    auto exe_dir = std::string{ fs.GetExeDir() };
    fs.Mount(exe_dir + "/assets", "/assets");
    fs.SetAssetDir(exe_dir + "/assets");

    fs.Open("/assets/prefabs/testprefab.idp", FS_PERMISSIONS::WRITE);
    auto save_path = fs.GetFile("/assets/prefabs/testprefab.idp");
    PrefabUtility::Save(go, save_path);
}