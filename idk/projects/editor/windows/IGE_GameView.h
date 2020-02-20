#pragma once
#include <idk.h>
#include <editor/windows/IGE_IWindow.h>
#include <imgui/imgui.h>

namespace idk
{
	class IGE_GameView
		: public IGE_IWindow
	{
	public:
		IGE_GameView();

		virtual void BeginWindow() override;
		virtual void Update() override;

		vec2 GetScreenSize(); //To get the size of the frame being drawn onto the window.

	private:
		vec2 _draw_rect_offset;
		vec2 _draw_rect_size;
		bool _maximize_on_play = true;
	};
}