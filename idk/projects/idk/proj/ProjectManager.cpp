#include "stdafx.h"
#include "ProjectManager.h"
#include <serialize/serialize.h>
#include <util/ioutils.h>
#include <scene/SceneManager.h>
#include <reflect/reflect.h>
#include <core/ConfigurableSystem.h>
#include <IncludeSystems.h>

#include <fstream>
#include <filesystem>
namespace fs = std::filesystem;

namespace idk
{

	void ProjectManager::Init()
	{
	}

	void ProjectManager::LateInit()
	{
	}

	void ProjectManager::Shutdown()
	{

	}

    void ProjectManager::LoadProject(string_view full_path)
    {
        auto& core_fs = Core::GetSystem<FileSystem>();
        core_fs.Dismount("/assets");
        core_fs.Dismount("/config");

        fs::path path = full_path;
        IDK_ASSERT_MSG(fs::exists(path), "Project does not exist!");
        _full_path = full_path;

        fs::path dir = path.parent_path();
        _project_dir = dir.string();
        _project_name = path.stem().string();

        fs::path assets = dir / "Assets";
        if (!fs::exists(assets))
            fs::create_directory(assets);
        _asset_dir = assets.string();
        core_fs.Mount(_asset_dir, "/assets");

        fs::path configs = dir / "Config";
        if (!fs::exists(configs))
            fs::create_directory(configs);
        core_fs.Mount(configs.string(), "/config", false);

        LoadConfigs();
    }

    void ProjectManager::SaveProject()
    {
    }

    template<typename T>
    static void load_config_for()
    {
        if constexpr (is_configurable_system<T>::value)
        {
            string_view name = reflect::get_type<T>().name();
            auto pos = name.rfind(':');
            if (pos != string_view::npos)
                name.remove_prefix(pos);
            string path = "/config/";
            path += name;
            path += ".idconf";

            typename T::Config config;
            auto stream = Core::GetSystem<FileSystem>().Open(path, FS_PERMISSIONS::READ);
            if (stream)
            {
                parse_text(stringify(stream), config);
                Core::GetSystem<T>().SetConfig(config);
            }
        }
    }

    template<size_t... Is>
    static void for_each_system_load_config(std::index_sequence<Is...>)
    {
        (load_config_for<std::tuple_element_t<Is, Systems>>(), ...);
    }

    void ProjectManager::LoadConfigs()
    {
        // initialize project
        auto scene_stream = Core::GetSystem<FileSystem>().Open("/config/SceneManager.idconf", FS_PERMISSIONS::READ);
        if (scene_stream)
            parse_text(stringify(scene_stream), Core::GetSystem<SceneManager>());

        for_each_system_load_config(std::make_index_sequence<std::tuple_size_v<Systems>>());
    }

    template<typename T>
    static void save_config_for()
    {
        if constexpr (is_configurable_system<T>::value)
        {
            string_view name = reflect::get_type<T>().name();
            auto pos = name.rfind(':');
            if (pos != string_view::npos)
                name.remove_prefix(pos);
            string path = "/config/";
            path += name;
            path += ".idconf";

            Core::GetSystem<FileSystem>().Open(path, FS_PERMISSIONS::WRITE) << serialize_text(Core::GetSystem<T>().GetConfig());
        }
    }

    template<size_t... Is>
    static void for_each_system_save_config(std::index_sequence<Is...>)
    {
        (save_config_for<std::tuple_element_t<Is, Systems>>(), ...);
    }

    void ProjectManager::SaveConfigs()
    {
        Core::GetSystem<FileSystem>().Open("/config/SceneManager.idconf", FS_PERMISSIONS::WRITE) << serialize_text(Core::GetSystem<SceneManager>());
        for_each_system_save_config(std::make_index_sequence<std::tuple_size_v<Systems>>());
    }
}
