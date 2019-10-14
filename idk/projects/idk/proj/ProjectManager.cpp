#include "stdafx.h"
#include "ProjectManager.h"
#include <serialize/serialize.h>
#include <util/ioutils.h>
#include <scene/SceneManager.h>

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
		// initialize project
		auto scene_stream = Core::GetSystem<FileSystem>().Open("/assets/scenes.idk", FS_PERMISSIONS::READ); 
		if (scene_stream)
			parse_text(stringify(scene_stream), Core::GetSystem<SceneManager>());
	}

	void ProjectManager::Shutdown()
	{

	}

	void ProjectManager::SaveProject()
	{
		Core::GetSystem<FileSystem>().Open("/assets/scenes.idk", FS_PERMISSIONS::WRITE) << serialize_text(Core::GetSystem<SceneManager>());
	}

    void ProjectManager::LoadProject(string_view full_path)
    {
        // @TODO: unmount "/assets" if already mounted

        auto& core_fs = Core::GetSystem<FileSystem>();
        fs::path path = full_path;
        IDK_ASSERT_MSG(fs::exists(path), "Project does not exist!");

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
        core_fs.Mount(configs.string(), "/config");

        fs::path recent_path = core_fs.GetAppDataDir();
        recent_path /= "idk";
        if (!fs::exists(recent_path))
            fs::create_directory(recent_path);

        recent_path /= ".recent";
        std::ofstream recent_file{ recent_path };
        recent_file << full_path;
    }

    string ProjectManager::GetRecentProjectPath() const
    {
        fs::path recent_path = Core::GetSystem<FileSystem>().GetAppDataDir();
        recent_path /= "idk";
        recent_path /= ".recent";
        if (!fs::exists(recent_path))
            return "";
        std::ifstream recent_file{ recent_path };
        fs::path proj = stringify(recent_file);
        if (!fs::exists(proj))
            return "";
        return proj.string();
    }
}
