#pragma once
#include <res/ResourceHandle.h>

namespace idk
{
	void NewScene();
	bool OpenScene();
    bool OpenScene(RscHandle<Scene> scene);
	void SaveScene();
	void SaveSceneAs();

	void SaveSceneTemporarily();
	void RestoreFromTemporaryScene();

	void HotReloadDLL();
}