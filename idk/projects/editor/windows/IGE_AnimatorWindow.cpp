#include "pch.h"
#include <core/Core.h>
#include <anim/AnimationUtils.h>

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
				resetSelection();
			_curr_animator_component = curr_obj_comp;
		}
		
		ImGui::Columns(3);
		if (ImGui::IsWindowAppearing())
			ImGui::SetColumnWidth(-1, 200);
		
		drawLeftCol();
		ImGui::NextColumn();

		if (ImGui::IsWindowAppearing())
			ImGui::SetColumnWidth(-1, 400);

		drawCanvas();

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
		auto window_pos = ImGui::GetWindowPos();
		canvasContextMenu();
		

		ImGui::ShowDemoWindow();
		ImNodes::BeginCanvas(&_canvas);
		

		ImNodes::EndCanvas();
		ImGui::EndChild();
	}

	void IGE_AnimatorWindow::drawLayersTab()
	{
		static char buf[50];
		static bool just_rename = false;
		static int rename_index = -1;

		static const float variable_offset = 20.0f;
		static const ImVec2 selectable_offset{ 0, 5 };
		static const ImVec2 layer_display_size{ 0, 100 };
		
		static const ImVec4 blend_prog_col{ 0.386953115f,0.759855568f, 0.793749988f, 1.0f };

		static const ImVec4 selectable_bg_col = ImVec4{ ImGui::GetStyleColorVec4(ImGuiCol_Header).x, 
														ImGui::GetStyleColorVec4(ImGuiCol_Header).y, 
														ImGui::GetStyleColorVec4(ImGuiCol_Header).z, 
														ImGui::GetStyleColorVec4(ImGuiCol_Header).w * 0.3f };
		static const ImVec4 selectable_active_col = ImVec4{ ImGui::GetStyleColorVec4(ImGuiCol_HeaderActive).x,
															ImGui::GetStyleColorVec4(ImGuiCol_HeaderActive).y, 
															ImGui::GetStyleColorVec4(ImGuiCol_HeaderActive).z, 
															ImGui::GetStyleColorVec4(ImGuiCol_HeaderActive).w * 0.3f };
		static const ImVec4 selectable_hovered_col = ImVec4{	ImGui::GetStyleColorVec4(ImGuiCol_HeaderHovered).x,
																ImGui::GetStyleColorVec4(ImGuiCol_HeaderHovered).y,
																ImGui::GetStyleColorVec4(ImGuiCol_HeaderHovered).z,
																ImGui::GetStyleColorVec4(ImGuiCol_HeaderHovered).w * 0.3f };
		
		if (ImGui::BeginTabItem("Layers", nullptr, ImGuiTabItemFlags_NoCloseWithMiddleMouseButton))
		{
			if (_curr_animator_component)
			{
				
				for (int i =0 ;i < _curr_animator_component->layers.size(); ++i)
				{
					auto& layer = _curr_animator_component->layers[i];
					strcpy_s(buf, layer.name.data());
					ImGui::PushID(i);
					
					auto prev_cursor_pos = ImGui::GetCursorPos();
					const bool selected = i == _selected_layer_index;
					ImGui::PushStyleColor(ImGuiCol_Header, selectable_bg_col);
					ImGui::PushStyleColor(ImGuiCol_HeaderActive, selectable_active_col);
					ImGui::PushStyleColor(ImGuiCol_HeaderHovered, selectable_hovered_col);
					ImGui::PushItemFlag(ImGuiItemFlags_Disabled, selected);
					if (ImGui::Selectable("##layer_selectable", selected, ImGuiSelectableFlags_AllowItemOverlap, ImVec2{ 0, 100 }))
						_selected_layer_index = i;
					ImGui::PopItemFlag();
					ImGui::PopStyleColor(3);
					auto new_cursor_pos = ImGui::GetCursorPos();

					// Draw layer variables
					
					ImGui::SetCursorPos(prev_cursor_pos + selectable_offset);
					ImGui::Indent(10);
					if (rename_index == i)
					{
						if (ImGui::InputText("##rename", buf, 50, ImGuiInputTextFlags_AutoSelectAll | ImGuiInputTextFlags_NoUndoRedo | ImGuiInputTextFlags_EnterReturnsTrue))
						{
							LOG_TO(LogPool::ANY, "RENAMED");

							rename_index = -1;
						}
						if (just_rename)
						{
							ImGui::SetKeyboardFocusHere(-1);
							just_rename = false;
						}
						else if (ImGui::IsItemDeactivated())
							rename_index = -1;
					}
					else
					{
						ImGui::Text(buf);
						if (ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(0))
						{
							rename_index = i;
							just_rename = true;
						}
					}

					ImGui::Text("Blending");
					ImGui::SameLine(0.0f, variable_offset);
					if (ImGui::BeginCombo("##layer_blend_mode", layer.blend_type.to_string().data()))
					{
						if (ImGui::Selectable("Override", layer.blend_type == AnimLayerBlend::Override_Blend))
							layer.blend_type = AnimLayerBlend::Override_Blend;
						if (ImGui::Selectable("Additive (Not supported)", layer.blend_type == AnimLayerBlend::Additive_Blend))
							layer.blend_type = AnimLayerBlend::Override_Blend;
						ImGui::EndCombo();
					}

					ImGui::Text("Weight");
					ImGui::SameLine(0.0f, variable_offset);
					if (layer.IsPlaying())
						ImGui::DragFloat("##def weight", &layer.weight, 0.01f, 0.0f, 1.0f, "%.2f");
					else
						ImGui::DragFloat("##def weight", &layer.default_weight, 0.01f, 0.0f, 1.0f, "%.2f");

					ImGui::NewLine();
					if (layer.IsPlaying())
						ImGui::ProgressBar(layer.weight, ImVec4{ 0.5, 0.5, 0.5, 1.0 }, ImVec2(-1, 3), nullptr);
					else
						ImGui::ProgressBar(layer.default_weight, ImVec4{ 0.5, 0.5, 0.5, 1.0 }, ImVec2(-1, 3), nullptr);

					ImGui::Unindent();
					ImGui::SetCursorPos(new_cursor_pos);
					// if (ImGui::InputText("##rename", buf, 50, ImGuiInputTextFlags_AutoSelectAll | ImGuiInputTextFlags_NoUndoRedo | ImGuiInputTextFlags_EnterReturnsTrue))
					// {
					// 	LOG_TO(LogPool::ANY, "RENAMED");
					// }
					// 
					// if(layer.IsPlaying())
					// 	ImGui::DragFloat("##def weight", &layer.weight, 0.01f, 0.0f, 1.0f, "%.2f");
					// else
					// 	ImGui::DragFloat("##def weight", &layer.default_weight, 0.01f, 0.0f, 1.0f, "%.2f");
					// 
					// ImGui::NewLine();
					// ImGui::ProgressBar(layer.curr_state.normalized_time, ImVec2(-1, 3), nullptr);
					// if(layer.blend_duration > 0.0001f)
					// 	ImGui::ProgressBar(layer.blend_state.normalized_time / layer.blend_duration, blend_prog_col, ImVec2(-1, 3), nullptr);
					// else
					// 	ImGui::ProgressBar(layer.blend_state.normalized_time, blend_prog_col, ImVec2(-1, 3), nullptr);
					// 
					// if (layer.IsPlaying())
					// 	ImGui::ProgressBar(layer.weight, ImVec4{ 0.5, 0.5, 0.5, 1.0 }, ImVec2(-1, 3), nullptr);
					// else
					// 	ImGui::ProgressBar(layer.default_weight, ImVec4{ 0.5, 0.5, 0.5, 1.0 }, ImVec2(-1, 3), nullptr);

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
	void IGE_AnimatorWindow::canvasContextMenu()
	{
		ImGui::SetWindowFontScale(1.0f);
		if (!ImGui::IsMouseDragPastThreshold(1) && ImGui::IsMouseReleased(1) && !ImGui::IsAnyItemHovered() && ImGui::IsWindowHovered())
		{
			ImGui::OpenPopup("canvas_context_menu");
		}
		// if (ImGui::IsPopupOpen("canvas_context_menu"))
		// 	ImGui::SetNextWindowSizeConstraints(ImVec2{ 200, 320 }, ImVec2{ 200, 320 });
		if (ImGui::BeginPopup("canvas_context_menu"))
		{
			if (ImGui::Selectable("Create State"))
			{
				_curr_animator_component->layers[_selected_layer_index].AddAnimation(RscHandle<anim::Animation>{});
				ImGui::CloseCurrentPopup();
			}

			if (ImGui::Selectable("Create Blend Tree"))
			{
				_curr_animator_component->layers[_selected_layer_index].AddAnimation(RscHandle<anim::Animation>{});
				// auto& new_anim = _curr_animator_component->layers[_selected_layer_index].anim_states.back();
				// new_anim.state_data = 
				ImGui::CloseCurrentPopup();
			}
			
			ImGui::EndPopup();
		}
	}
	void IGE_AnimatorWindow::resetSelection()
	{
		_display_mode = None;
		_selected_layer_index = 0;
		_selected_state = -1;
	}
}