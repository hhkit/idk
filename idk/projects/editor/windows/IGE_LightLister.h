#pragma once

#include <editor/windows/IGE_IWindow.h>

namespace idk
{
	class IGE_LightLister :
		public IGE_IWindow
	{
	public:
		IGE_LightLister();

		virtual void BeginWindow() override;
		virtual void Update() override;

	};
}