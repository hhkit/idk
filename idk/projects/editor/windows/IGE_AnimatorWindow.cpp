#include "pch.h"
#include <core/Core.h>
#include <anim/AnimationUtils.h>
#include "imguidk.h"

#include "imguidk.h"
#include "IGE_AnimatorWindow.h"
#include "IGE_InspectorWindow.h"
#include "IDE.h"

namespace idk
{
	
	IGE_AnimatorWindow::IGE_AnimatorWindow()
		:IGE_IWindow{ "Animator##IGE_AnimatorWindow",true, ImVec2{ 800,300 }, ImVec2{ 0,0 } } 
	{		
		// window_flags = ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse | ImGuiWindowFlags_MenuBar;;
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

		_blend_prog_col = ImVec4{ 0.386953115f,0.759855568f, 0.793749988f, 1.0f };
		_layer_selectable_display_size = ImVec2{ 0, 100 };
		_state_selectable_display_size = ImVec2{ 0, 40 };
		_selectable_offset = ImVec2{ 0, 5 };

		_selectable_bg_col = ImVec4{ ImGui::GetStyleColorVec4(ImGuiCol_Header).x,
									ImGui::GetStyleColorVec4(ImGuiCol_Header).y,
									ImGui::GetStyleColorVec4(ImGuiCol_Header).z,
									ImGui::GetStyleColorVec4(ImGuiCol_Header).w * 0.3f };
		_selectable_active_col = ImVec4{ ImGui::GetStyleColorVec4(ImGuiCol_HeaderActive).x,
										ImGui::GetStyleColorVec4(ImGuiCol_HeaderActive).y,
										ImGui::GetStyleColorVec4(ImGuiCol_HeaderActive).z,
										ImGui::GetStyleColorVec4(ImGuiCol_HeaderActive).w * 0.3f };
		_selectable_hovered_col = ImVec4{ ImGui::GetStyleColorVec4(ImGuiCol_HeaderHovered).x,
											ImGui::GetStyleColorVec4(ImGuiCol_HeaderHovered).y,
											ImGui::GetStyleColorVec4(ImGuiCol_HeaderHovered).z,
											ImGui::GetStyleColorVec4(ImGuiCol_HeaderHovered).w * 0.3f };
		

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

		// float window_size = ImGui::GetWindowWidth();
		ImGui::Columns(3);
		if (ImGui::IsWindowAppearing())
			ImGui::SetColumnWidth(-1, window_size.x * 0.25f);
		
		drawLeftCol();
		ImGui::NextColumn();

		if (ImGui::IsWindowAppearing())
			ImGui::SetColumnWidth(-1, window_size.x * 0.375f);

		drawCanvas();

		ImGui::NextColumn();

		drawAnimatorInspector();
		// if (ImGui::IsWindowAppearing())
		// 	ImGui::SetColumnWidth(-1, 400);
	}
	IGE_AnimatorWindow::~IGE_AnimatorWindow()
	{
	}

	void IGE_AnimatorWindow::separator()
	{
		ImDrawList* draw_list = ImGui::GetWindowDrawList();
		ImVec2 p = ImGui::GetCursorScreenPos();
		draw_list->AddLine(ImVec2(p.x - 9999, p.y), ImVec2(p.x + 9999, p.y), ImGui::GetColorU32(ImGuiCol_Border));
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

		auto tab_flags = ImGuiTabBarFlags_NoCloseWithMiddleMouseButton;
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
		ImGui::BeginChild("Canvas", ImVec2(0, 0), false, ImGuiWindowFlags_AlwaysUseWindowPadding);
		auto tab_flags = ImGuiTabBarFlags_NoCloseWithMiddleMouseButton;
		if (ImGui::BeginTabBar("States & Bone Mask", tab_flags))
		{
			drawStatesTab();
			
			ImGui::EndTabBar();
		};
		ImGui::EndChild();
		//ImGui::BeginChild("Canvas", ImVec2(0, 0), false, ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse);
		//auto window_pos = ImGui::GetWindowPos();
		//if (canvasContextMenu())
		//{
		//	// New state was created, need to set the positions
		//	auto& new_state = _curr_animator_component->layers[_selected_layer].anim_states.back();
		//	new_state.node_position = (ImGui::GetWindowPos() - window_pos - _canvas.offset) / _canvas.zoom;
		//}
		//

		//// ImGui::ShowDemoWindow();
		//ImNodes::BeginCanvas(&_canvas);
		//
		//if (_curr_animator_component)
		//{
		//	// drawConnection(window_pos + _curr_animator_component->layers[0].anim_states[1].node_position, _curr_animator_component->layers[0].anim_states[2].node_position);
		//	auto& layer = _curr_animator_component->layers[_selected_layer];

		//	for (size_t i = 1; i < layer.anim_states.size(); ++i)
		//	{
		//		auto& state = layer.anim_states[i];
		//		bool selected_state = _selected_state == i;
		//		bool was_selected = selected_state;
		//		if (ImNodes::BeginNode(&state, reinterpret_cast<ImVec2*>(&state.node_position), &selected_state))
		//		{
		//			ImGui::SetCursorPosX(ImGui::GetCursorPosX() + ImGui::GetStyle().ItemSpacing.x * 0.5f);
		//			ImGui::Text(state.name.c_str());
		//			ImGui::NewLine();
		//			// ImGui::DragFloat2("Node Pos", &state.node_position[0], 0.01f);
		//			float prog_bar = 0.0f;
		//			if (layer.curr_state.index == i)
		//				prog_bar = layer.curr_state.normalized_time;
		//			else if (layer.blend_state.index == i)
		//				prog_bar = layer.blend_state.normalized_time;
		//			ImGui::ProgressBar(prog_bar, ImVec2(100, 3), nullptr);

		//		}
		//		ImNodes::EndNode();

		//		if (!was_selected && selected_state)
		//		{
		//			_selected_state = i;
		//			_show_transition = false;
		//			_display_mode = AnimatorDisplayMode::State;
		//		}

		//		
		//	}
		//}
		//ImNodes::EndCanvas();
		//ImGui::EndChild();
	}

	void IGE_AnimatorWindow::drawAnimatorInspector()
	{
		if (_curr_animator_component)
		{
			static char buf[50];
			auto& curr_layer = _curr_animator_component->layers[_selected_layer];
			auto& curr_state = curr_layer.anim_states[_selected_state];

			const auto display_transition = [&]()
			{
				if (_selected_transition < 0)
					return;

				ImGui::PushID(_selected_transition);
				auto& curr_transition = curr_state.transitions[_selected_transition];
				auto& transition_to_state = curr_layer.anim_states[curr_transition.transition_to_index];
				ImGui::Text("Transition To: ");
				ImGui::SameLine();
				if (ImGui::BeginCombo("##transition_to", transition_to_state.name.data()))
				{
					for (size_t i = 0; i < curr_layer.anim_states.size(); ++i)
					{
						auto& s = curr_layer.anim_states[i];
						bool selected = i == curr_transition.transition_to_index;
						if (ImGui::Selectable(s.name.data(), selected))
						{
							curr_transition.transition_to_index = i;
						}
					}

					ImGui::EndCombo();
				}

				if (!transition_to_state.valid)
					ImGuidk::PushDisabled();
				ImGui::Text("Has Exit Time");
				ImGui::SameLine();
				ImGui::Checkbox("##has_exit_time", &curr_transition.has_exit_time);

				// Exit time must be positive
				ImGui::Text("Exit Time");
				ImGui::SameLine();
				if (ImGui::InputFloat("##exit_time", &curr_transition.exit_time))
					curr_transition.exit_time = max(curr_transition.exit_time, 0.0f);
				ImGui::NewLine();

				ImGui::Text("Transition Duration");
				ImGui::SameLine();
				if (ImGui::InputFloat("##transition_dur", &curr_transition.transition_duration))
					curr_transition.transition_duration = max(curr_transition.transition_duration, 0.0f);
				ImGui::Text("Transition Offset");
				ImGui::SameLine();
				if (ImGui::InputFloat("##transition_offset", &curr_transition.transition_offset))
					curr_transition.transition_offset = max(curr_transition.transition_offset, 0.0f);

				if (!transition_to_state.valid)
					ImGuidk::PopDisabled();

				ImGui::NewLine();
				separator();
				ImGui::NewLine();
				ImGui::PopID();
			};
			
			const auto display_state = [&]()
			{
				constexpr float transition_max_indent = 50.0f;
				
				if (_selected_state == 0)
					return;


				auto& state_data = *curr_state.GetBasicState();

				ImGui::PushID(_selected_state);
				strcpy_s(buf, curr_state.name.data());
				
				ImGui::Text("Animation State");
				float width_avail = (ImGui::GetContentRegionAvailWidth() - ImGui::CalcTextSize("Animation State").x);
				float item_width = width_avail * 0.8f;
				float offset = ImGui::CalcTextSize("Animation State").x + width_avail - item_width;

				ImGui::SameLine(offset);
				ImGui::PushItemWidth(item_width);
				if (ImGui::InputText("##name", buf, ImGuiInputTextFlags_AutoSelectAll | ImGuiInputTextFlags_NoUndoRedo | ImGuiInputTextFlags_EnterReturnsTrue))
				{
					curr_layer.RenameAnimation(curr_state.name, buf);
				}
				ImGui::PopItemWidth();

				const bool has_valid_clip = s_cast<bool>(state_data.motion);
				!has_valid_clip ? ImGui::TextColored(ImVec4{ 1,0,0,1 }, "Clip") : ImGui::Text("Clip");

				ImGui::SameLine(offset);
				ImGui::PushItemWidth(item_width);
				ImGuidk::InputResource("##clip", &state_data.motion);
				ImGui::PopItemWidth();
				if (!has_valid_clip)
					ImGuidk::PushDisabled();

				ImGui::Text("Speed");
				ImGui::SameLine(offset);
				ImGui::PushItemWidth(item_width);
				ImGui::DragFloat("##speed", &curr_state.speed, 0.01f);
				ImGui::PopItemWidth();

				ImGui::Text("Loop");
				ImGui::SameLine(offset);
				ImGui::Checkbox("##loop", &curr_state.loop);
				ImGui::NewLine();
				separator();

				ImGui::Text("Transitions");
				ImGui::PushItemWidth(-1);
				auto bg_col = ImGui::GetStyle().Colors[ImGuiCol_FrameBg];
				bg_col.w = bg_col.w * 0.4f;
				ImGui::PushStyleColor(ImGuiCol_FrameBg, bg_col);
				const bool is_open = ImGui::ListBoxHeader("##transitions", curr_state.transitions.size(), 5);
				ImGui::PopStyleColor();
				if (is_open)
				{
					for (size_t i = 0; i < curr_state.transitions.size(); ++i)
					{
						ImGui::PushID(i);
						auto& transition = curr_state.transitions[i];
						bool selected = _selected_transition == i;
						string transition_from = curr_layer.GetAnimationState(transition.transition_from_index).name;
						string transition_to = curr_layer.GetAnimationState(transition.transition_to_index).name;
						string transition_title = transition_from + "\t->\t" + transition_to;


						if (ImGui::Selectable(transition_title.data(), &selected))
						{
							_show_transition = true;
							_selected_transition = i;
						}

						if (ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(0))
						{
							_display_mode = AnimatorDisplayMode::Transition;
							_selected_transition = i;
						}
						ImGui::PopID();
					}

					ImGui::ListBoxFooter();
				}
				ImGui::PopItemWidth();
				
				if (ImGui::Button("+"))
				{
					curr_state.AddTransition(_selected_state, 0);
				}
				
				if (!has_valid_clip)
					ImGuidk::PopDisabled();

				ImGui::NewLine();
				separator();
				ImGui::NewLine();
				ImGui::PopID();

				if (_show_transition)
					display_transition();
			};

			switch (_display_mode)
			{
			case AnimatorDisplayMode::State:
			{
				display_state();
				break;
			}
			case AnimatorDisplayMode::Transition:
			{
				display_transition();
				break;
			}
			// case AnimatorDisplayMode::BlendTree:
			// {
			// 
			// 	break;
			// }
			case AnimatorDisplayMode::None:
				break;
			}
		}
	}

	void IGE_AnimatorWindow::drawLayersTab()
	{
		static char buf[50];
		static bool just_rename = false;
		static int rename_index = -1;
		
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
					const bool selected = i == _selected_layer;
					ImGui::PushStyleColor(ImGuiCol_Header, _selectable_bg_col);
					ImGui::PushStyleColor(ImGuiCol_HeaderActive, _selectable_active_col);
					if(selected)
						ImGui::PushStyleColor(ImGuiCol_HeaderHovered, _selectable_bg_col);
					else
						ImGui::PushStyleColor(ImGuiCol_HeaderHovered, _selectable_hovered_col);
					if (ImGui::Selectable("##layer_selectable", selected, ImGuiSelectableFlags_AllowItemOverlap, _layer_selectable_display_size))
					{
						resetSelection();
						_selected_layer = i;
						_display_mode = AnimatorDisplayMode::Layer;
					}
					ImGui::PopStyleColor(3);
					separator();
					auto new_cursor_pos = ImGui::GetCursorPos();
					ImGui::Indent(ImGui::GetStyle().FramePadding.x);
					// Draw layer variables
					ImGui::SetCursorPos(prev_cursor_pos + _selectable_offset);
					if (rename_index == i)
					{
						if (ImGui::InputText("##rename", buf, 50, ImGuiInputTextFlags_AutoSelectAll | ImGuiInputTextFlags_NoUndoRedo | ImGuiInputTextFlags_EnterReturnsTrue))
						{
							LOG_TO(LogPool::ANY, "RENAMED");
							_curr_animator_component->RenameLayer(layer.name, buf);
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
					
					const float width_avail = (ImGui::GetContentRegionAvailWidth() - ImGui::CalcTextSize("Blending").x);
					const float item_width = width_avail * 0.8f;
					const float offset = ImGui::CalcTextSize("Blending").x + width_avail - item_width;

					ImGui::SameLine(offset);
					ImGui::PushItemWidth(item_width);
					if (ImGui::BeginCombo("##layer_blend_mode", layer.blend_type.to_string().data()))
					{
						if (ImGui::Selectable("Override", layer.blend_type == AnimLayerBlend::Override_Blend))
							layer.blend_type = AnimLayerBlend::Override_Blend;
						if (ImGui::Selectable("Additive (Not supported)", layer.blend_type == AnimLayerBlend::Additive_Blend))
							layer.blend_type = AnimLayerBlend::Override_Blend;
						ImGui::EndCombo();
					}
					ImGui::PopItemWidth();

					ImGui::Text("Weight");
					ImGui::SameLine(offset);
					ImGui::PushItemWidth(item_width);
					if (layer.IsPlaying())
						ImGui::DragFloat("##def weight", &layer.weight, 0.01f, 0.0f, 1.0f, "%.2f");
					else
						ImGui::DragFloat("##def weight", &layer.default_weight, 0.01f, 0.0f, 1.0f, "%.2f");
					ImGui::PopItemWidth();

					ImGui::NewLine();
					const ImVec2 prog_size = ImVec2(ImGui::GetContentRegionAvailWidth() - ImGui::GetStyle().FramePadding.x * 2, 3);
					if (layer.IsPlaying())
						ImGui::ProgressBar(layer.weight, ImVec4{ 0.5, 0.5, 0.5, 1.0 }, prog_size, nullptr);
					else
						ImGui::ProgressBar(layer.default_weight, ImVec4{ 0.5, 0.5, 0.5, 1.0 }, ImVec2(-1, 3), nullptr);
					
					ImGui::Unindent(ImGui::GetStyle().FramePadding.x);
					ImGui::SetCursorPos(new_cursor_pos);
					
					ImGui::PopID();
					
				}

				ImGui::NewLine();
				if (ImGui::Button("Add Layer"))
				{
					_curr_animator_component->AddLayer();
				}
				ImGui::SameLine();

				if (_curr_animator_component->layers.size() <= 1)
					ImGuidk::PushDisabled();

				if (ImGui::Button("Delete Layer"))
				{
					_curr_animator_component->RemoveLayer(_selected_layer);
				}

				if (_curr_animator_component->layers.size() <= 1)
					ImGuidk::PopDisabled();

			}
			ImGui::EndTabItem();
		}
		
	}
	void IGE_AnimatorWindow::drawParamsTab()
	{

		if (ImGui::BeginTabItem("Parameters", nullptr, ImGuiTabItemFlags_NoCloseWithMiddleMouseButton))
		{
			if (_curr_animator_component)
			{
				auto& int_vars = _curr_animator_component->int_vars;
				auto& float_vars = _curr_animator_component->float_vars;
				auto& bool_vars = _curr_animator_component->bool_vars;
				auto& trigger_vars = _curr_animator_component->trigger_vars;

				const float width_avail = (ImGui::GetContentRegionAvailWidth() - ImGui::CalcTextSize("Add").x);
				const float item_width = width_avail * 0.8f;
				const float offset = ImGui::CalcTextSize("Blending").x + width_avail - item_width;

				ImGui::Text("Float");
				ImGui::SameLine(5);
				if (ImGui::Button("+"))
				{
					_curr_animator_component->SetFloat("New Float", 0.0f, true);
				}
				separator();

				for (size_t i = 0; i < int_vars.size(); ++i)
				{
					// ImGui::InputText("##float_name");
				}
				
				ImGui::NewLine();

				ImGui::Text("Int");
				ImGui::SameLine(5);
				if (ImGui::Button("+"))
				{
					_curr_animator_component->SetInt("New Float", 0, true);
				}
				ImGui::SameLine();
				if (ImGui::Button("+"))
				{
					_curr_animator_component->SetInt("New Float", 0, true);
				}
				separator();
				ImGui::NewLine();

				ImGui::Text("Bool");
				ImGui::SameLine(5);
				if (ImGui::Button("+"))
				{
					_curr_animator_component->SetBool("New Bool", false, true);
				}
				separator();
				ImGui::NewLine();

				ImGui::Text("Trigger");
				ImGui::SameLine(5);
				if (ImGui::Button("+"))
				{
					_curr_animator_component->SetTrigger("New Trigger", false, true);
				}
				separator();
				ImGui::NewLine();
			}
			
			ImGui::EndTabItem();
		}
	}
	void IGE_AnimatorWindow::drawStatesTab()
	{
		static char buf[50];
		static bool just_rename = false;
		static int rename_index = -1;

		static const ImVec2 selectable_offset{ 0, 5 };

		if (ImGui::BeginTabItem("States", nullptr, ImGuiTabItemFlags_NoCloseWithMiddleMouseButton))
		{
			if (_curr_animator_component)
			{
				auto& curr_layer = _curr_animator_component->layers[_selected_layer];
				for (int i = 1; i < curr_layer.anim_states.size(); ++i)
				{
					auto& state = curr_layer.anim_states[i];
					strcpy_s(buf, state.name.data());
					ImGui::PushID(i);

					auto prev_cursor_pos = ImGui::GetCursorPos();
					const bool selected = i == _selected_state;
					ImGui::PushStyleColor(ImGuiCol_Header, _selectable_bg_col);
					ImGui::PushStyleColor(ImGuiCol_HeaderActive, _selectable_active_col);
					if (selected)
						ImGui::PushStyleColor(ImGuiCol_HeaderHovered, _selectable_bg_col);
					else
						ImGui::PushStyleColor(ImGuiCol_HeaderHovered, _selectable_hovered_col);
					if (ImGui::Selectable("##state_selectable", selected, ImGuiSelectableFlags_AllowItemOverlap, _state_selectable_display_size))
					{
						const size_t curr_selected_layer = _selected_layer;
						resetSelection();
						_selected_layer = curr_selected_layer;
						_selected_state = i;
						_display_mode = AnimatorDisplayMode::State;
					}
					ImGui::PopStyleColor(3);
					separator();
					auto new_cursor_pos = ImGui::GetCursorPos();

					// Draw layer variables
					ImGui::SetCursorPos(prev_cursor_pos + selectable_offset);
					ImGui::Indent(ImGui::GetStyle().FramePadding.x);
					if (rename_index == i)
					{
						if (ImGui::InputText("##rename_state", buf, 50, ImGuiInputTextFlags_AutoSelectAll | ImGuiInputTextFlags_NoUndoRedo | ImGuiInputTextFlags_EnterReturnsTrue))
						{
							curr_layer.RenameAnimation(state.name, buf);
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

					float prog_bar = 0.0f;
					if (curr_layer.curr_state.index == i)
						prog_bar = curr_layer.curr_state.normalized_time;
					else if (curr_layer.blend_state.index == i)
						prog_bar = curr_layer.blend_state.normalized_time;

					ImGui::ProgressBar(prog_bar, ImVec2(-1, 3), nullptr);
					ImGui::Unindent(ImGui::GetStyle().FramePadding.x);
					ImGui::SetCursorPos(new_cursor_pos);

					ImGui::PopID();
					
				}

				ImGui::NewLine();
				if (ImGui::Button("Add State"))
				{
					curr_layer.AddAnimation(RscHandle<anim::Animation>{});
				}
				ImGui::SameLine();

				const bool can_remove = _selected_state != 0 && curr_layer.anim_states.size() > 1;
				if (!can_remove)
					ImGuidk::PushDisabled();
				if (ImGui::Button("Delete State"))
				{
					curr_layer.RemoveAnimation(_selected_state);
				}
				if (!can_remove)
					ImGuidk::PopDisabled();
			}
			ImGui::EndTabItem();
		}
	}
	bool IGE_AnimatorWindow::canvasContextMenu()
	{
		bool result = false;
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
				_curr_animator_component->layers[_selected_layer].AddAnimation(RscHandle<anim::Animation>{});
				result = true;
				ImGui::CloseCurrentPopup();
			}

			if (ImGui::Selectable("Create Blend Tree"))
			{
				_curr_animator_component->layers[_selected_layer].AddAnimation(RscHandle<anim::Animation>{});
				result = true;
				// auto& new_anim = _curr_animator_component->layers[_selected_layer_index].anim_states.back();
				// new_anim.state_data = 
				ImGui::CloseCurrentPopup();
			}
			
			ImGui::EndPopup();
		}
		return result;
	}
	//bool IGE_AnimatorWindow::drawConnection(ImVec2 p1, ImVec2 p2)
	//{
	//	static ImVec2 triangle_a{}, triangle_b, triangle_c;
	//	vec2 v1 = p2 - p1;
	//	vec2 v1_norm = v1.get_normalized();
	//	vec2 mid_point = p1 + (v1 / 2.0f);

	//	ImDrawList* draw_list = ImGui::GetWindowDrawList();
	//	const ImGuiStyle& style = ImGui::GetStyle();
	//	float connection_indent = _canvas.style.connection_indent * _canvas.zoom;
	//	auto win_pos = ImGui::GetWindowPos();
	//	// p1 = (p1 - _canvas.prev_win_pos - _canvas.prev_offset) / _canvas.prev_zoom * _canvas.zoom + win_pos + _canvas.offset;
	//	// p2 = (p2 - _canvas.prev_win_pos - _canvas.prev_offset) / _canvas.prev_zoom * _canvas.zoom + win_pos + _canvas.offset;
	//	// p1.x += connection_indent;
	//	// p2.x -= connection_indent;

	//	float thickness = 10.0f;// _canvas.style.curve_thickness* _canvas.zoom;
	//	
	//	draw_list->AddLine(p1 + win_pos, p2 + win_pos, _canvas.colors[ImNodes::ColConnection]);
	//	
	//	
	//	float min_square_distance = FLT_MAX;
	//	for (int i = 0; i < draw_list->_Path.size() - 1; i++)
	//	{
	//		min_square_distance = min(min_square_distance,
	//			ImNodes::GetDistanceToLineSquared(ImGui::GetMousePos(), draw_list->_Path[i], draw_list->_Path[i + 1]));
	//	}

	//	// Draw curve, change when it is hovered
	//	bool is_close = min_square_distance <= thickness * thickness && ImGui::IsWindowHovered(ImGuiHoveredFlags_AllowWhenBlockedByActiveItem);
	//	
	//	// draw_list->PathStroke(ImColor{ 1,0,0,1 }, false , thickness);

	//	

	//	return is_close;
	//}

	void IGE_AnimatorWindow::resetSelection()
	{
		_display_mode = None;
		_selected_layer = 0;
		_selected_state = 0;
		_selected_transition = -1;
		_show_transition = false;
	}
}