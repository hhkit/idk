#include "pch.h"
#include "IGE_GameView.h"
#include <editor/IDE.h>
#include <res/ResourceHandle.inl>
#include <gfx/RenderTarget.h>

namespace idk
{
	IGE_GameView::IGE_GameView()
		:IGE_IWindow{ "Game",true,ImVec2{ 800,600 },ImVec2{ 0,50 } } {		//Delegate Constructor to set window size
			// We are using the ImGuiWindowFlags_NoDocking flag to make the parent window not dockable into,
		// because it would be confusing to have two docking targets within each others.
		window_flags = ImGuiWindowFlags_NoCollapse
			| ImGuiWindowFlags_NoScrollWithMouse
			| ImGuiWindowFlags_NoScrollbar
			| ImGuiWindowFlags_MenuBar;
	}

	void IGE_GameView::BeginWindow()
	{
		ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
		ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
	}

	void IGE_GameView::Update()
	{
		ImGui::PopStyleVar(3);

		if (ImGui::BeginMenuBar())
		{
			ImGui::SetCursorPosX(ImGui::GetWindowContentRegionWidth() -
								 ImGui::CalcTextSize("Maximize On Play").x - ImGui::GetStyle().FramePadding.y * 2 -
								 ImGui::GetTextLineHeight() - ImGui::GetStyle().ItemSpacing.x * 2);
			ImGui::Checkbox("Maximize On Play", &_maximize_on_play);
			ImGui::EndMenuBar();
		}

		if (_maximize_on_play && Core::GetSystem<IDE>().IsGameRunning())
			Core::GetSystem<IDE>().MaximizeWindow(this);
		else
			Core::GetSystem<IDE>().MaximizeWindow(nullptr);

		auto imageSize = vec2{ GetScreenSize() };
		auto meta = RscHandle<RenderTarget>{};
		auto screen_tex = meta->GetColorBuffer();
		auto rendertex_aspect = screen_tex->AspectRatio();
		auto windowframe_aspect = imageSize.x / imageSize.y;
		// game view fits the texture to the window
		if (rendertex_aspect > windowframe_aspect) // rendertex is horizontally longer
			imageSize.y = imageSize.x / rendertex_aspect;
		else if (rendertex_aspect < windowframe_aspect) // windowframe is horizontally longer
			imageSize.x = imageSize.y * rendertex_aspect;
		_draw_rect_size = imageSize;

		_draw_rect_offset = (GetScreenSize() - imageSize) * 0.5f;
		ImGui::SetCursorPos(vec2(ImGui::GetWindowContentRegionMin()) + _draw_rect_offset);

		//imageSize.y = (imageSize.x * (9 / 16));
		//if (Core::GetSystem<GraphicsSystem>().GetAPI() != GraphicsAPI::Vulkan)
		ImGui::Image(screen_tex->ID(), imageSize, ImVec2(0, 1), ImVec2(1, 0));
	}
	vec2 IGE_GameView::GetScreenSize()
	{
		return vec2{ ImGui::GetWindowContentRegionMax() } - vec2{ ImGui::GetWindowContentRegionMin() };
	}
}
