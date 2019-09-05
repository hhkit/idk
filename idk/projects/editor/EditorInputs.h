#pragma once

#include <gfx/CameraControls.h>

namespace idk
{
	class EditorInputs
	{
	public:

		void Update();

		// main camera (Public for now)
		CameraControls main_camera;
	private:

	};
};
