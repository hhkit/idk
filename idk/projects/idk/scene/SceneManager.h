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
		Signal<RscHandle<Scene>> OnSceneChange;

		RscHandle<Scene>   _startup_scene; // public for reflection to use, please don't touch
		RscHandle<Scene>   _active_scene;  // public for reflection to use, please don't touch

		// accessors
		RscHandle<Scene>       GetSceneByBuildIndex(unsigned char index) const;
		
		// properties
		RscHandle<Scene>    StartupScene() const;
		// true if successfully set, false otherwise
		bool                StartupScene(RscHandle<Scene> scene);

		RscHandle<Scene> GetActiveScene() const;
		RscHandle<Scene> GetPrefabScene() const;
		bool  SetActiveScene(RscHandle<Scene> s);

		void DestroyQueuedObjects(span<GameObject> objs);

		void BuildSceneGraph(span<const GameObject> objs);
		SceneGraph& FetchSceneGraph();
		SceneGraph* FetchSceneGraphFor(Handle<class GameObject>);

	private:
		friend class Scene;
		void Init() override;
		void LateInit() override;
		void EarlyShutdown() override;
		void Shutdown() override;

		GameState*         _gs { nullptr };
		SceneGraphBuilder  _sg_builder;
		RscHandle<Scene>   _prefab_scene;
		array<RscHandle<Scene>, 0x82> _scenes;
	};
}