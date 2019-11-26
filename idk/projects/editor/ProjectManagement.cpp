#include "pch.h"
#include "ProjectManagement.h"
#include <file/FileSystem.h>
#include <util/ioutils.h>
#include <app/Application.h>
#include <proj/ProjectManager.h>
#include <editor/IDE.h>

namespace idk
{
    void LoadRecentProject()
    {
        namespace fs = std::filesystem;

        auto& proj_manager = Core::GetSystem<ProjectManager>();
        auto& recent = Core::GetSystem<IDE>().recent;
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
    }

}
