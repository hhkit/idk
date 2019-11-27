#include "pch.h"
#include "ProjectManagement.h"
#include <file/FileSystem.h>
#include <util/ioutils.h>
#include <app/Application.h>
#include <proj/ProjectManager.h>
#include <editor/IDE.h>
#include <editor/SceneManagement.h>
#include <gfx/Camera.h>
#include <core/GameObject.h>
#include <common/Transform.h>

namespace idk
{
    void LoadRecentProject()
    {
        namespace fs = std::filesystem;

        auto& proj_manager = Core::GetSystem<ProjectManager>();
        Registry recent{ "/idk/.recent" };
        string proj_name = recent.get("project");

        if (proj_name.empty())
        {
            const DialogOptions dialog{ "IDK Project", ProjectManager::ext };
            auto proj = Core::GetSystem<Application>().OpenFileDialog(dialog);
            while (!proj)
                proj = Core::GetSystem<Application>().OpenFileDialog(dialog);
            proj_manager.LoadProject(*proj);
        }
        else
            proj_manager.LoadProject(proj_name);

        recent.set("project", string{ proj_manager.GetProjectFullPath() });

        fs::path user_dir = proj_manager.GetProjectDir();
        user_dir /= "User";
        if (!fs::exists(user_dir))
            fs::create_directory(user_dir);

        Core::GetSystem<FileSystem>().Dismount("/user");
        Core::GetSystem<FileSystem>().Mount(user_dir.string(), "/user", false);
    }

}
