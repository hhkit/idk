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
			| ImGuiWindowFlags_NoScrollbar;
	}

	void IGE_GameView::BeginWindow()
	{
	}

	void IGE_GameView::Update()
	{
		ImVec2 imageSize = GetScreenSize();
		auto meta = RscHandle<RenderTarget>{};
		auto screen_tex = meta->GetColorBuffer();
		auto rendertex_aspect = screen_tex->AspectRatio();
		auto windowframe_aspect = imageSize.x / imageSize.y;
		// game view fits the texture to the window
		if (rendertex_aspect > windowframe_aspect) // rendertex is horizontally longer
			imageSize.y = imageSize.x / rendertex_aspect;
		else if (rendertex_aspect < windowframe_aspect) // windowframe is horizontally longer
			imageSize.x = imageSize.y * rendertex_aspect;
		draw_rect_size = imageSize;

		draw_rect_offset = (GetScreenSize() - imageSize) * 0.5f;
		ImGui::SetCursorPos(vec2(ImGui::GetWindowContentRegionMin()) + draw_rect_offset);

		//imageSize.y = (imageSize.x * (9 / 16));
		//if (Core::GetSystem<GraphicsSystem>().GetAPI() != GraphicsAPI::Vulkan)
		ImGui::Image(screen_tex->ID(), imageSize, ImVec2(0, 1), ImVec2(1, 0));
	}
	vec2 IGE_GameView::GetScreenSize()
	{
		return vec2{ ImGui::GetWindowContentRegionMax() } -ImGui::GetWindowContentRegionMin();
	}
}
