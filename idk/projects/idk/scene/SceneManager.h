#pragma once
#include <idk.h>
#include <idk_config.h>
#include <core/ISystem.h>
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
		void Init() override;
		void Shutdown() override;

		RscHandle<Scene> CreateScene(string_view name = "");
		RscHandle<Scene> LoadScene(uint8_t scene_id, LoadSceneMode = LoadSceneMode::Single);
		//Scene LoadScene(string_view path_to_scene, LoadSceneMode = LoadSceneMode::Single);
		RscHandle<Scene> GetActiveScene();
		bool  SetActiveScene(RscHandle<Scene> s);

		string_view GetSceneName();
	private:
		GameState* _gs = nullptr;
		RscHandle<Scene> _active_scene;
	};
}