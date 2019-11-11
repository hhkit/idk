#pragma once
#include <editor/windows/IGE_IWindow.h>

namespace idk 
{
	class IGE_ProfilerWindow :
		public IGE_IWindow
	{
	public:
		IGE_ProfilerWindow();

		virtual void BeginWindow() override;
		virtual void Update() override;

	};





}