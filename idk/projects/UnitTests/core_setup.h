#pragma once

#include <core/Core.inl>
#include "TestApplication.h"
#include <file/FileSystem.h>
#include <debug/LogSystem.h>

static inline void core_setup(idk::Core& core)
{
    core.AddSystem<TestApplication>();
    core.GetSystem<idk::LogSystem>().SetLogDir(TEST_BIN_DIR "/logs");
    core.Setup();
}

static inline idk::FileSystem& mount_test_assets()
{
    auto& fs = idk::Core::GetSystem<idk::FileSystem>();
    fs.Mount(TEST_DATA_PATH, "/assets");
    return fs;
}

#define INIT_CORE() idk::Core core; core_setup(core)
#define INIT_ASSETS() auto& fs = mount_test_assets()