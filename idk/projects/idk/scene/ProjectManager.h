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

		SceneActivateResult ActivateScene(RscHandle<class Scene>);

		void                SaveProject();

		RscHandle<Scene>    StartupScene() const;
		// true if successfully set, false otherwise
		bool                StartupScene(RscHandle<Scene> scene);

		span<const SceneBlock> GetScenes() const;
	private:
		friend class SceneFactory;

		vector<SceneBlock>     _scenes;
		RscHandle<class Scene> _startup_scene;

		void Init() override;
		void LateInit() override;
		void Shutdown() override;

		unique_ptr<Scene> CreateScene();

	};
}