#pragma once
#include <idk.h>
#include <idk_config.h>
#include <core/ISystem.h>
#include <scene/SceneGraphBuilder.h>
namespace idk
{
	enum class LoadSceneMode
	{
		Single,
		Additive
	};

	class SceneManager
		: public ISystem
	{
	public:
		using SceneGraph = SceneGraphBuilder::SceneGraph;
		struct iterator;

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

		// properties
		RscHandle<Scene>    StartupScene() const;
		// true if successfully set, false otherwise
		bool                StartupScene(RscHandle<Scene> scene);

		RscHandle<Scene> GetActiveScene();
		bool  SetActiveScene(RscHandle<Scene> s);

		void DestroyObjects();

		void BuildSceneGraph(span<const GameObject> objs);
		SceneGraph& FetchSceneGraph();
		SceneGraph* FetchSceneGraphFor(Handle<class GameObject>);

	private:
		void Init() override;
		void Shutdown() override;

		GameState* _gs = nullptr;
		vector<SceneBlock>     _scenes;
		RscHandle<class Scene> _startup_scene;
		RscHandle<Scene>       _active_scene;
		SceneGraphBuilder      _sg_builder;
	};
}