#include "pch.h"
#include "IGE_GameView.h"
#include <editor/IDE.h>
#include <res/ResourceHandle.inl>
#include <gfx/RenderTarget.h>
#include <ui/RectTransform.h>
#include <ui/UISystem.h>
#include <ui/Canvas.h>
#include <scene/SceneManager.h>
#include <scene/SceneGraph.inl>
#include <math/matrix_transforms.inl>

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

	static rect get_global_rect(Handle<RectTransform> rt)
	{
		if (!rt)
			return {};
		const auto parent = rt->GetGameObject()->Parent();
		if (!parent)
			return rt->_local_rect;
		const auto parent_rt = parent->GetComponent<RectTransform>();
		const rect parent_rect = parent->GetComponent<Canvas>() ? parent_rt->_local_rect : get_global_rect(parent_rt);
		return rect{
			parent_rect.position + parent_rect.size * parent_rt->pivot + rt->_local_rect.position,
			rt->_local_rect.size
		};
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

		const auto image_top_left = ImGui::GetCursorScreenPos();

		//imageSize.y = (imageSize.x * (9 / 16));
		//if (Core::GetSystem<GraphicsSystem>().GetAPI() != GraphicsAPI::Vulkan)
		ImGui::Image(screen_tex->ID(), imageSize, ImVec2(0, 1), ImVec2(1, 0));

		const auto& sel = Core::GetSystem<IDE>().GetSelectedObjects();
		if (sel.game_objects.size() && sel.game_objects[0] && sel.game_objects[0]->HasComponent<RectTransform>())
		{
			const auto canvas = Core::GetSystem<UISystem>().FindCanvas(sel.game_objects[0]);
			if (!canvas)
				return;

			const auto canvas_size = canvas->GetGameObject()->GetComponent<RectTransform>()->_local_rect.size;
			const auto canvas_scl = imageSize / canvas_size;

			const auto rt = sel.game_objects[0]->GetComponent<RectTransform>();

			rect global_rect = get_global_rect(rt);
			rect parent_global_rect = get_global_rect(rt->GetGameObject()->Parent()->GetComponent<RectTransform>());

			{ // draw anchor reference rect
				vec2 a = parent_global_rect.position + parent_global_rect.size * rt->anchor_min;
				vec2 b = parent_global_rect.position + parent_global_rect.size * rt->anchor_max;
				a.y = canvas_size.y - a.y; // imgui draws from top to bottom
				b.y = canvas_size.y - b.y;
				a = a * canvas_scl + image_top_left;
				b = b * canvas_scl + image_top_left;
				ImGui::GetWindowDrawList()->AddRect(a, b, 0xFFFFFFFF);

				// draw anchors
				const float x = 8.0f, y = 4.0f;
				const float u = idk::min(a.y, b.y);
				const float d = idk::max(a.y, b.y);
				const float l = idk::min(a.x, b.x);
				const float r = idk::max(a.x, b.x);
				const vec2 ul{ l, u }, ur{ r, u }, dl{ l, d }, dr{ r, d };
				ImGui::GetWindowDrawList()->AddTriangle(ul, ul + vec2{ -x, -y }, ul + vec2{ -y, -x }, 0xFFFFFFFF);
				ImGui::GetWindowDrawList()->AddTriangle(ur, ur + vec2{  x, -y }, ur + vec2{  y, -x }, 0xFFFFFFFF);
				ImGui::GetWindowDrawList()->AddTriangle(dl, dl + vec2{ -x,  y }, dl + vec2{ -y,  x }, 0xFFFFFFFF);
				ImGui::GetWindowDrawList()->AddTriangle(dr, dr + vec2{  x,  y }, dr + vec2{  y,  x }, 0xFFFFFFFF);
			}
			{ // draw actual rect
				vec2 a = global_rect.position;
				vec2 b = global_rect.position + global_rect.size;
				a.y = canvas_size.y - a.y; // imgui draws from top to bottom
				b.y = canvas_size.y - b.y;
				a = a * canvas_scl + image_top_left;
				b = b * canvas_scl + image_top_left;
				ImGui::GetWindowDrawList()->AddRect(a, b, 0xFFFF9999);
			}
		}
	}

	vec2 IGE_GameView::GetScreenSize()
	{
		return vec2{ ImGui::GetWindowContentRegionMax() } - vec2{ ImGui::GetWindowContentRegionMin() };
	}



}
