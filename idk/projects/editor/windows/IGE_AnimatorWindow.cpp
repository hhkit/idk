#include "pch.h"
#include <core/Core.h>

#include "IGE_AnimatorWindow.h"
#include "IGE_InspectorWindow.h"
#include "IDE.h"

namespace idk
{

	IGE_AnimatorWindow::IGE_AnimatorWindow()
		:IGE_IWindow{ "Animator##IGE_AnimatorWindow",true, ImVec2{ 800,300 }, ImVec2{ 0,0 } } 
	{		
		window_flags = ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse | ImGuiWindowFlags_MenuBar;;
	}
	void IGE_AnimatorWindow::BeginWindow()
	{
	}
	void IGE_AnimatorWindow::Initialize()
	{
		_canvas.colors[ImNodes::ColNodeBg] = ImGui::GetColorU32(ImGuiCol_Border);
		_canvas.colors[ImNodes::ColNodeBg].Value.w = 0.75f;
		_canvas.colors[ImNodes::ColNodeActiveBg] = ImGui::GetColorU32(ImGuiCol_Border);
		_canvas.colors[ImNodes::ColNodeActiveBg].Value.w = 0.75f;
		_canvas.style.curve_thickness = 2.5f;
		_canvas.drag_button = 2;
		_canvas.scroll_to_move = false;
		_canvas.no_ctrl_to_zoom = true;
		_canvas.drag_zoom = false;
	}
	void IGE_AnimatorWindow::Update()
	{
		if (!is_open || ImGui::IsWindowCollapsed())
		{
			ImNodes::BeginCanvas(&_canvas);
			ImNodes::EndCanvas();
			return;
		}
		
		if (Core::GetSystem<IDE>().selected_gameObjects.empty())
		{
			_curr_animator_component = Handle<Animator>{};
		}
		else
		{
			const auto curr_obj = Core::GetSystem<IDE>().selected_gameObjects[0];
			const auto curr_obj_comp = curr_obj->GetComponent<Animator>();
			if (curr_obj_comp != _curr_animator_component)
				_display_mode = None;
			_curr_animator_component = curr_obj_comp;
		}
		
		ImGui::Columns(3);
		if (ImGui::IsWindowAppearing())
			ImGui::SetColumnWidth(-1, 200);
		
		drawLeftCol();
		ImGui::NextColumn();

		if (ImGui::IsWindowAppearing())
			ImGui::SetColumnWidth(-1, 400);

		

		ImGui::NextColumn();

		// if (ImGui::IsWindowAppearing())
		// 	ImGui::SetColumnWidth(-1, 400);
	}
	IGE_AnimatorWindow::~IGE_AnimatorWindow()
	{
	}
	void IGE_AnimatorWindow::drawLeftCol()
	{
		// ImGui::SetCursorPos(ImGui::GetWindowContentRegionMin());

		ImGui::PushStyleColor(ImGuiCol_ChildBg, ImGui::GetStyleColorVec4(ImGuiCol_WindowBg));
		ImGui::PushStyleVar(ImGuiStyleVar_ChildRounding, 2.0f);

		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 4.0f));
		ImGui::BeginChild("LeftChild", ImVec2(), false, ImGuiWindowFlags_AlwaysUseWindowPadding);
		ImGui::PopStyleVar(2);
		ImGui::PopStyleColor();

		auto tab_flags = ImGuiTabBarFlags_NoCloseWithMiddleMouseButton | ImGuiTabBarFlags_FittingPolicyScroll;
		if(ImGui::BeginTabBar("Layers & Params", tab_flags))
		{
			
			drawLayersTab();
			drawParamsTab();

			ImGui::EndTabBar();
		};


		ImGui::EndChild();
	}
	void IGE_AnimatorWindow::drawCanvas()
	{
		ImGui::BeginChild("Canvas", ImVec2(0, 0), false, ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse);
		ImNodes::BeginCanvas(&_canvas);

		ImNodes::EndCanvas();
		ImGui::EndChild();
	}

	void IGE_AnimatorWindow::drawLayersTab()
	{
		static char buf[50];
		static ImVec4 blend_prog_col{ 0.386953115f,0.759855568f, 0.793749988f, 1.0f };

		if (ImGui::BeginTabItem("Layers", nullptr, ImGuiTabItemFlags_NoCloseWithMiddleMouseButton))
		{
			if (_curr_animator_component)
			{
				
				int i = 0;
				for (auto& layer : _curr_animator_component->layers)
				{
					strcpy_s(buf, layer.name.data());
					ImGui::PushID(i++);
					if (ImGui::InputText("##rename", buf, 50, ImGuiInputTextFlags_AutoSelectAll | ImGuiInputTextFlags_NoUndoRedo | ImGuiInputTextFlags_EnterReturnsTrue))
					{
						LOG_TO(LogPool::ANY, "RENAMED");
					}

					if(layer.IsPlaying())
						ImGui::DragFloat("##def weight", &layer.weight, 0.01f, 0.0f, 1.0f, "%.2f");
					else
						ImGui::DragFloat("##def weight", &layer.default_weight, 0.01f, 0.0f, 1.0f, "%.2f");

					ImGui::NewLine();
					ImGui::ProgressBar(layer.curr_state.normalized_time, ImVec2(-1, 3), nullptr);
					if(layer.blend_duration > 0.0001f)
						ImGui::ProgressBar(layer.blend_state.normalized_time / layer.blend_duration, blend_prog_col, ImVec2(-1, 3), nullptr);
					else
						ImGui::ProgressBar(layer.blend_state.normalized_time, blend_prog_col, ImVec2(-1, 3), nullptr);

					if (layer.IsPlaying())
						ImGui::ProgressBar(layer.weight, ImVec4{ 0.5, 0.5, 0.5, 1.0 }, ImVec2(-1, 3), nullptr);
					else
						ImGui::ProgressBar(layer.default_weight, ImVec4{ 0.5, 0.5, 0.5, 1.0 }, ImVec2(-1, 3), nullptr);

					ImGui::PopID();
					ImGui::Separator();
				}

				if (ImGui::Button("Add Layer"))
				{
					_curr_animator_component->AddLayer();
				}
			}
			ImGui::EndTabItem();
		}
		
	}
	void IGE_AnimatorWindow::drawParamsTab()
	{
		if (ImGui::BeginTabItem("Parameters", nullptr, ImGuiTabItemFlags_NoCloseWithMiddleMouseButton))
		{
			ImGui::EndTabItem();
		}
	}
}