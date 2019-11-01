#pragma once

namespace idk
{
	void NewScene();
	bool OpenScene();
	void SaveScene();
	void SaveSceneAs();

	void SaveSceneTemporarily();
	void RestoreFromTemporaryScene();

	void HotReloadDLL();
}