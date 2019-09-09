#pragma once
#include <idk.h>
#include <core/ISystem.h>
#include <scene/Scene.h>

namespace idk
{
	class Scene;

	class ProjectManager
		: public ISystem
	{
	public:
		static constexpr string_view project_path = "/assets/idk.proj";

		struct SceneBlock
		{
			unsigned char          build_index;
			RscHandle<class Scene> scene;
		};

		enum class SceneActivateResult
		{
			Ok = 0,
			Err_SceneAlreadyActive,
			Err_SceneNotInProject,
			Err_ScenePathNotFound
		};
		// accessors
		RscHandle<Scene>       GetSceneByBuildIndex(unsigned char index) const;
		span<const SceneBlock> GetScenes() const;

		// modifiers
		SceneActivateResult ActivateScene(RscHandle<class Scene>);
		RscHandle<Scene>    CreateScene();

		void                SaveProject();

		// properties
		RscHandle<Scene>    StartupScene() const;
		// true if successfully set, false otherwise
		bool                StartupScene(RscHandle<Scene> scene);

	private:
		vector<SceneBlock>     _scenes;
		RscHandle<class Scene> _startup_scene;

		void Init() override;
		void LateInit() override;
		void Shutdown() override;


	};
}