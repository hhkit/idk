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
		void Init() override;
		void Shutdown() override;

		RscHandle<Scene> CreateScene(string_view name = "");
		RscHandle<Scene> LoadScene(uint8_t scene_id, LoadSceneMode = LoadSceneMode::Single);
		//Scene LoadScene(string_view path_to_scene, LoadSceneMode = LoadSceneMode::Single);
		RscHandle<Scene> GetActiveScene();
		bool  SetActiveScene(RscHandle<Scene> s);

		void DestroyObjects();

		void BuildSceneGraph(span<const GameObject> objs);
		SceneGraph& FetchSceneGraph();
		SceneGraph* FetchSceneGraphFor(Handle<class GameObject>);

		string_view GetSceneName();
	private:
		GameState* _gs = nullptr;
		SceneGraphBuilder _sg_builder;
		RscHandle<Scene> _active_scene;
	};
}