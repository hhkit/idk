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
		Signal<> OnProjectSaved;

        void LoadProject(string_view full_path);
		void SaveProject();

        void LoadConfigs();
        void SaveConfigs();

        string_view GetProjectFullPath() const { return _full_path; }
        string_view GetProjectName() const { return _project_name; }
        string_view GetProjectDir() const { return _project_dir; }
        string_view GetAssetDir() const { return _asset_dir; }

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