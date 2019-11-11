#pragma once

#include <editor/windows/IGE_IWindow.h>

namespace idk {
	class IGE_AnimatorWindow :
		public IGE_IWindow
	{
	public:
		IGE_AnimatorWindow();

		virtual void BeginWindow() override;
		virtual void Update() override;
	};
}