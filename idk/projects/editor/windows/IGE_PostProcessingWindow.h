#pragma once

#include <editor/windows/IGE_IWindow.h>

namespace idk
{
	class IGE_PostProcessingWindow :
		public IGE_IWindow
	{
	public:
		IGE_PostProcessingWindow();

		virtual void BeginWindow() override;
		virtual void Update() override;

	};
}