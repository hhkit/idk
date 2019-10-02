#pragma once
#include <idk.h>
#include <idk_config.h>
#include <core/ISystem.h>
#include <scene/SceneGraphBuilder.h>
namespace idk
{
	class Scene;

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

		vector<SceneBlock> _scenes;        // public for reflection to use, please don't touch
		RscHandle<Scene>   _startup_scene; // public for reflection to use, please don't touch
		RscHandle<Scene>   _active_scene;  // public for reflection to use, please don't touch

		// accessors
		RscHandle<Scene>       GetSceneByBuildIndex(unsigned char index) const;
		span<const SceneBlock> GetScenes() const;

		// modifiers
		RscHandle<Scene>    CreateScene();

		// properties
		RscHandle<Scene>    StartupScene() const;
		// true if successfully set, false otherwise
		bool                StartupScene(RscHandle<Scene> scene);

		RscHandle<Scene> GetActiveScene();
		bool  SetActiveScene(RscHandle<Scene> s);

		void DestroyObjects(span<GameObject> objs);

		void BuildSceneGraph(span<const GameObject> objs);
		SceneGraph& FetchSceneGraph();
		SceneGraph* FetchSceneGraphFor(Handle<class GameObject>);

	private:
		void Init() override;
		void LateInit() override;
		void Shutdown() override;

		GameState*         _gs { nullptr };
		SceneGraphBuilder  _sg_builder;
	};
}