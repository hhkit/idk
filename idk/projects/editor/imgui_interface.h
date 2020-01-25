#pragma once

namespace idk
{
	class imgui_interface
	{
	public:
		imgui_interface() = default;
		virtual ~imgui_interface() = default;

		virtual void Init() = 0;
		virtual void Shutdown() = 0;

		virtual void ImGuiFrameBegin() = 0;
		virtual void ImGuiFrameUpdate() = 0;
		virtual void ImGuiFrameEnd() = 0;

		virtual void ImGuiFrameRender() = 0;
	};
}