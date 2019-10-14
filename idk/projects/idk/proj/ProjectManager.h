#pragma once
#include <idk.h>
#include <core/ISystem.h>
#include <scene/Scene.h>

namespace idk
{
	class ProjectManager
		: public ISystem
	{
	public:
		static constexpr string_view ext = ".idk";

		void SaveProject();
        void LoadProject(string_view full_path);
        string GetRecentProjectPath() const;

        string_view GetProjectName() { return _project_name; }
        string_view GetProjectDir() { return _project_dir; }
        string_view GetAssetDir() { return _asset_dir; }

	private:
        string _full_path;
        string _project_name;
        string _project_dir;
        string _asset_dir;

		void Init() override;
		void LateInit() override;
		void Shutdown() override;
	};
}