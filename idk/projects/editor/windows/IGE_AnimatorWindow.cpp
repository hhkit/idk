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
		:IGE_IWindow{ "Animator##IGE_AnimatorWindow",true, ImVec2{ 900,600 }, ImVec2{ 0,0 } } 
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
		_layer_selectable_num_items = 3;
		_state_selectable_num_items = 1;

		_selectable_bg_col = ImVec4{ ImGui::GetStyleColorVec4(ImGuiCol_Header).x,
									ImGui::GetStyleColorVec4(ImGuiCol_Header).y,
									ImGui::GetStyleColorVec4(ImGuiCol_Header).z,
									ImGui::GetStyleColorVec4(ImGuiCol_Header).w * 0.4f };
		_selectable_active_col = ImVec4{ ImGui::GetStyleColorVec4(ImGuiCol_HeaderActive).x,
										ImGui::GetStyleColorVec4(ImGuiCol_HeaderActive).y,
										ImGui::GetStyleColorVec4(ImGuiCol_HeaderActive).z,
										ImGui::GetStyleColorVec4(ImGuiCol_HeaderActive).w * 0.4f };
		_selectable_hovered_col = ImVec4{ ImGui::GetStyleColorVec4(ImGuiCol_HeaderHovered).x,
											ImGui::GetStyleColorVec4(ImGuiCol_HeaderHovered).y,
											ImGui::GetStyleColorVec4(ImGuiCol_HeaderHovered).z,
											ImGui::GetStyleColorVec4(ImGuiCol_HeaderHovered).w * 0.4f };
		

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
		
		layersAndParams();
		ImGui::NextColumn();

		if (ImGui::IsWindowAppearing())
			ImGui::SetColumnWidth(-1, window_size.x * 0.375f);

		statesAndBoneMask();

		ImGui::NextColumn();

		animatorInspector();
		// if (ImGui::IsWindowAppearing())
		// 	ImGui::SetColumnWidth(-1, 400);
	}
	IGE_AnimatorWindow::~IGE_AnimatorWindow()
	{
	}

	void IGE_AnimatorWindow::arrow()
	{
		const auto arrow_pos = ImVec2{ ImGui::GetCurrentWindowRead()->DC.CursorPos.x, 
									   ImGui::GetCurrentWindowRead()->DC.CursorPos.y + ImGui::GetFontSize() * 0.25f };
		ImGui::RenderArrow(arrow_pos, ImGuiDir_Right, 0.7f);
	}

	void IGE_AnimatorWindow::layersAndParams()
	{
		// ImGui::SetCursorPos(ImGui::GetWindowContentRegionMin());

		ImGui::PushStyleColor(ImGuiCol_ChildBg, ImGui::GetStyleColorVec4(ImGuiCol_WindowBg));
		ImGui::PushStyleVar(ImGuiStyleVar_ChildRounding, 2.0f);

		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 4.0f));
		ImGui::BeginChild("LeftChild", ImVec2(), false, ImGuiWindowFlags_AlwaysUseWindowPadding);
		ImGui::PopStyleVar(2);
		ImGui::PopStyleColor();

		const auto tab_flags = ImGuiTabBarFlags_NoCloseWithMiddleMouseButton;
		if(ImGui::BeginTabBar("Layers & Params", tab_flags))
		{
			
			drawLayersTab();
			drawParamsTab();
			
			ImGui::EndTabBar();
		};


		ImGui::EndChild();
	}
	void IGE_AnimatorWindow::statesAndBoneMask()
	{
		ImGui::PushStyleColor(ImGuiCol_ChildBg, ImGui::GetStyleColorVec4(ImGuiCol_WindowBg));
		ImGui::PushStyleVar(ImGuiStyleVar_ChildRounding, 2.0f);

		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 4.0f));
		ImGui::BeginChild("MiddleChild", ImVec2(), false, ImGuiWindowFlags_AlwaysUseWindowPadding);
		ImGui::PopStyleVar(2);
		ImGui::PopStyleColor();

		auto tab_flags = ImGuiTabBarFlags_NoCloseWithMiddleMouseButton;
		if (ImGui::BeginTabBar("States & Bone Mask", tab_flags))
		{
			drawStatesTab();
			
			ImGui::EndTabBar();
		};
		ImGui::EndChild();
	}

	void IGE_AnimatorWindow::drawLayersTab()
	{
		static char buf[50];
		static bool just_rename = false;
		static int rename_index = -1;

		static const ImVec2 selectable_offset{ 0, ImGui::GetStyle().FramePadding.y };
		static const ImVec2 selectable_size{ 0, ImGui::GetFrameHeightWithSpacing() * _layer_selectable_num_items + 3.0f + ImGui::GetStyle().FramePadding.y * 4 };
		
		if (ImGui::BeginTabItem("Layers", nullptr, ImGuiTabItemFlags_NoCloseWithMiddleMouseButton))
		{
			if (_curr_animator_component)
			{
				if (ImGui::Button("Add"))
				{
					_curr_animator_component->AddLayer();
				}
				ImGui::SameLine();

				if (_curr_animator_component->layers.size() <= 1)
					ImGuidk::PushDisabled();

				if (ImGui::Button("Delete"))
				{
					_curr_animator_component->RemoveLayer(_selected_layer);
					_selected_layer = std::max(size_t{ 0 }, _selected_layer - 1);
				}

				if (_curr_animator_component->layers.size() <= 1)
					ImGuidk::PopDisabled();

				//ImGui::NewLine();

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
					if (ImGui::Selectable("##layer_selectable", selected, ImGuiSelectableFlags_AllowItemOverlap, selectable_size))
					{
						selectLayer(i);
					}
					ImGui::PopStyleColor(3);
					ImGui::Separator(false);

					// Save the current position so we can set the position back after we finish drawing the widgets
					auto new_cursor_pos = ImGui::GetCursorPos();

					// Indent and set position to draw starting point
					ImGui::SetCursorPos(prev_cursor_pos + selectable_offset);
					ImGui::Indent(ImGui::GetStyle().FramePadding.x);

					// Draw layer variables
					if (rename_index == i)
					{
						if (ImGui::InputText("##rename", buf, 50, ImGuiInputTextFlags_AutoSelectAll | ImGuiInputTextFlags_NoUndoRedo | ImGuiInputTextFlags_EnterReturnsTrue))
						{
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

					if (layer.default_index == 0)
					{
						ImGui::SameLine();
						ImGui::TextColored(ImVec4{ 1,0,0,1 }, " (!)");
						if (ImGui::IsItemHovered() && ImGui::IsWindowHovered())
						{
							ImGui::BeginTooltip();
							ImGui::Text("No default state selected!");
							ImGui::EndTooltip();
						}
					}
					
					float align_edit = ImGui::GetContentRegionAvailWidth() - ImGui::CalcTextSize("...").x - ImGui::GetStyle().FramePadding.x * 2;
					align_edit = std::max(align_edit, 0.0f);
					ImGui::SameLine(align_edit);

					if (ImGui::Button("..."))
					{
						ImGui::OpenPopup("layers_context_menu");
						selectLayer(i);
					}
					drawLayersContextMenu();

					//const float width_avail = (ImGui::GetContentRegionAvailWidth() - ImGui::CalcTextSize("Blending").x);
					//const float item_width = width_avail * 0.8f;
					//const float offset = ImGui::CalcTextSize("Blending").x + width_avail - item_width;
					//ImGui::Text("Blending");
					//ImGui::SameLine(offset);
					//ImGui::PushItemWidth(item_width);
					//if (ImGui::BeginCombo("##layer_blend_mode", layer.blend_type.to_string().data()))
					//{
					//	if (ImGui::Selectable("Override", layer.blend_type == AnimLayerBlend::Override_Blend))
					//		layer.blend_type = AnimLayerBlend::Override_Blend;
					//	if (ImGui::Selectable("Additive (Not supported)", layer.blend_type == AnimLayerBlend::Additive_Blend))
					//		layer.blend_type = AnimLayerBlend::Override_Blend;
					//	ImGui::EndCombo();
					//}
					//ImGui::PopItemWidth();
					//
					//ImGui::Text("Weight");
					//ImGui::SameLine(offset);
					//ImGui::PushItemWidth(item_width);
					//if (layer.IsPlaying())
					//	ImGui::DragFloat("##def weight", &layer.weight, 0.01f, 0.0f, 1.0f, "%.2f");
					//else
					//	ImGui::DragFloat("##def weight", &layer.default_weight, 0.01f, 0.0f, 1.0f, "%.2f");
					//ImGui::PopItemWidth();

					ImGui::NewLine();
					const ImVec2 prog_size = ImVec2(ImGui::GetContentRegionAvailWidth() - ImGui::GetStyle().FramePadding.x, 3);
					if (layer.IsPlaying())
						ImGui::ProgressBar(layer.weight, ImVec4{ 0.5, 0.5, 0.5, 1.0 }, prog_size, nullptr);
					else
						ImGui::ProgressBar(layer.default_weight, ImVec4{ 0.5, 0.5, 0.5, 1.0 }, ImVec2(-1, 3), nullptr);

					ImGui::Unindent(ImGui::GetStyle().FramePadding.x);

					ImGui::SetCursorPos(new_cursor_pos);
					
					ImGui::PopID();
					
				}


			}
			ImGui::EndTabItem();
		}
		
	}

	void IGE_AnimatorWindow::drawLayersContextMenu()
	{	
		if (ImGui::IsPopupOpen("layers_context_menu"))
		{
			ImGui::SetNextWindowSizeConstraints(ImVec2{ 300, 80 }, ImVec2{ 300, 80 });
			
		}
		if (ImGui::BeginPopup("layers_context_menu", ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoScrollWithMouse | ImGuiWindowFlags_NoResize))
		{
			auto& curr_layer = _curr_animator_component->layers[_selected_layer];
			auto& curr_def_state = curr_layer.GetAnimationState(curr_layer.default_index);

			const float width_avail = (ImGui::GetContentRegionAvailWidth() - ImGui::CalcTextSize("Entry State").x);
			const float item_width = width_avail * 0.8f;
			const float offset = ImGui::GetContentRegionAvailWidth() - item_width;

			const bool valid = curr_def_state.valid;
			valid ? ImGui::Text("Entry State") : ImGui::TextColored(ImVec4{ 1,0,0,1 }, "Entry State ");
			
			ImGui::SameLine(offset);
			ImGui::PushItemWidth(item_width);
			if (ImGui::BeginCombo("##entry_state", curr_def_state.name.c_str()))
			{
				for (size_t i = 1; i < curr_layer.anim_states.size(); ++i)
				{
					auto& state = curr_layer.anim_states[i];

					if (ImGui::Selectable(state.name.data(), i == curr_layer.default_index))
					{
						curr_layer.default_index = i;
						curr_layer.curr_state.index = i;
					}
				}
				ImGui::EndCombo();
			}
			ImGui::PopItemWidth();

			if (_selected_layer == 0)
				ImGuidk::PushDisabled();

			ImGui::Text("Weight ");
			static float no_edit_weight = 1.0f;
			auto& curr_weight = curr_layer.IsPlaying() ? curr_layer.weight : curr_layer.default_weight;
			ImGui::SameLine(offset);
			ImGui::PushItemWidth(item_width);
			ImGui::SliderFloat("##weight", _selected_layer == 0 ? &no_edit_weight : &curr_weight, 0.0f, 1.0f, "%.2f");
			ImGui::PopItemWidth();

			ImGui::Text("Blending");
			ImGui::SameLine(offset);
			ImGui::PushItemWidth(item_width);
			if (ImGui::BeginCombo("##layer_blend_mode", curr_layer.blend_type.to_string().data()))
			{
				if (ImGui::Selectable("Override", curr_layer.blend_type == AnimLayerBlend::Override_Blend))
					curr_layer.blend_type = AnimLayerBlend::Override_Blend;
				if (ImGui::Selectable("Additive (Not supported)", curr_layer.blend_type == AnimLayerBlend::Additive_Blend))
					curr_layer.blend_type = AnimLayerBlend::Override_Blend;
				ImGui::EndCombo();
			}
			ImGui::PopItemWidth();

			if (_selected_layer == 0)
				ImGuidk::PopDisabled();

			ImGui::EndPopup();
		}
	}

	void IGE_AnimatorWindow::drawParamsTab()
	{

		if (ImGui::BeginTabItem("Parameters", nullptr, ImGuiTabItemFlags_NoCloseWithMiddleMouseButton))
		{
			if (_curr_animator_component)
			{
				static char buf[50];
				string rename_to = "";

				static bool just_rename = false;
				static string rename_param = "";
				
				
				static const ImVec2 selectable_offset{ 0, ImGui::GetStyle().FramePadding.y };

				if (ImGui::Button("Add"))
				{
					ImGui::OpenPopup("params_context_menu");
				}
				drawParamsContextMenu();

				const bool can_delete = _selected_param_type != anim::AnimDataType::NONE || !_selected_param.empty();

				if (!can_delete)
					ImGuidk::PushDisabled();

				ImGui::SameLine();
				if (ImGui::Button("Delete"))
				{
					switch (_selected_param_type)
					{
					case anim::AnimDataType::INT:
					{
						_curr_animator_component->RemoveParam<anim::IntParam>(_selected_param);
						break;
					}
					case anim::AnimDataType::FLOAT:
					{
						_curr_animator_component->RemoveParam<anim::FloatParam>(_selected_param);
						break;
					}
					case anim::AnimDataType::BOOL:
					{
						_curr_animator_component->RemoveParam<anim::BoolParam>(_selected_param);
						break;
					}
					case anim::AnimDataType::TRIGGER:
					{
						_curr_animator_component->RemoveParam<anim::TriggerParam>(_selected_param);
						break;
					}
					default:
					{
						break;
					}
					}
				}

				if (!can_delete)
					ImGuidk::PopDisabled();
				
				//ImGui::NewLine();

				const auto display_param = [&](auto& param, bool selected) -> bool
				{
					using T = std::decay_t<decltype(param)>;
					bool ret_val = false;

					ImGui::PushID(&param);
					strcpy_s(buf, param.name.data());

					ImGui::Indent();

					ImGui::BeginGroup();
					
					auto prev_pos = ImGui::GetCursorPos();
					ImGui::PushStyleColor(ImGuiCol_Header, _selectable_bg_col);
					ImGui::PushStyleColor(ImGuiCol_HeaderActive, _selectable_active_col);
					if (selected)
						ImGui::PushStyleColor(ImGuiCol_HeaderHovered, _selectable_bg_col);
					else
						ImGui::PushStyleColor(ImGuiCol_HeaderHovered, _selectable_hovered_col);
					if (ImGui::Selectable("##param_selectable", selected, ImGuiSelectableFlags_AllowItemOverlap | ImGuiSelectableFlags_DrawFillAvailWidth))
					{
						ret_val = true;
						
					}
					ImGui::PopStyleColor(3);
					const auto next_pos = ImGui::GetCursorPos();
					ImGui::SetCursorPos(prev_pos + selectable_offset);

					const float align_widget = ImGui::GetContentRegionAvailWidth() * 0.7f;
					const float widget_size = ImGui::GetContentRegionAvailWidth() * 0.3f;

					ImGui::PushItemWidth(ImGui::GetContentRegionAvailWidth() * 0.5f);
					if (rename_param == param.name)
					{
						const bool renamed = ImGui::InputText("##rename_param", buf, 50, ImGuiInputTextFlags_AutoSelectAll | ImGuiInputTextFlags_NoUndoRedo | ImGuiInputTextFlags_EnterReturnsTrue);
						if (renamed)
						{
							rename_to = buf;
						}
						if (just_rename)
						{
							ImGui::SetKeyboardFocusHere(-1);
							just_rename = false;
						}
						else if (ImGui::IsItemDeactivated() && !renamed)
							rename_param.clear();
					}
					else
					{
						ImGui::Text(buf);
						if (ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(0))
						{
							rename_param = buf;
							just_rename = true;
						}
					}
					ImGui::PopItemWidth();

					ImGui::SameLine(align_widget);

					auto& val = _curr_animator_component->IsPlaying() ? param.val : param.def_val;
					if constexpr (std::is_same_v <T, anim::IntParam>)
					{
						ImGui::PushItemWidth(widget_size);
						ImGui::DragInt("##param_input", &val);
						ImGui::PopItemWidth();
					}
					else if constexpr (std::is_same_v < T, anim::FloatParam>)
					{
						ImGui::PushItemWidth(widget_size);
						ImGui::DragFloat("##param_input", &val, 0.01f);
						ImGui::PopItemWidth();
					}
					else if constexpr (std::is_same_v <T, anim::BoolParam>)
					{
						ImGui::Checkbox("##param_input", &val);
					}
					else if constexpr (std::is_same_v <T, anim::TriggerParam>)
					{
						if (ImGui::RadioButton("##param_input", val))
						{
							val = !val;
						}
					}
					else
						throw("???");

					ImGui::Separator(false);
					ImGui::SetCursorPos(next_pos);

					ImGui::EndGroup();
					ImGui::Unindent();
					ImGui::PopID();

					return ret_val;
				};

				ImGui::SetNextTreeNodeOpen(true, ImGuiCond_Once);
				if (ImGui::CollapsingHeader("Int", ImGuiTreeNodeFlags_Framed))
				// ImGui::Text("Int");
				// ImGui::Separator(false);
				{
					auto& param_table = _curr_animator_component->GetParamTable<anim::IntParam>();
					for (auto& param : param_table)
					{
						const bool change = display_param(param.second, _selected_param_type == anim::AnimDataType::INT && _selected_param == param.second.name);
						if (change)
						{
							_selected_param_type = anim::AnimDataType::INT;
							_selected_param = param.second.name;
						}
					}
					
					// Rename outside the loop
					if (!rename_to.empty())
					{
						_curr_animator_component->RenameParam<anim::IntParam>(rename_param, rename_to);
						rename_param.clear();
						rename_to.clear();
					}
				}

				ImGui::SetNextTreeNodeOpen(true, ImGuiCond_Once);
				if (ImGui::CollapsingHeader("Float", ImGuiTreeNodeFlags_Framed))
				// ImGui::Text("Float");
				// ImGui::Separator(false);
				{
					auto& param_table = _curr_animator_component->GetParamTable<anim::FloatParam>();
					for (auto& param : param_table)
					{
						const bool change = display_param(param.second, _selected_param_type == anim::AnimDataType::FLOAT && _selected_param == param.second.name);
						if (change)
						{
							_selected_param_type = anim::AnimDataType::FLOAT;
							_selected_param = param.second.name;
						}
					}

					// Rename outside the loop
					if (!rename_to.empty())
					{
						_curr_animator_component->RenameParam<anim::FloatParam>(rename_param, rename_to);
						rename_param.clear();
						rename_to.clear();
					}
				}

				ImGui::SetNextTreeNodeOpen(true, ImGuiCond_Once);
				if (ImGui::CollapsingHeader("Bool", ImGuiTreeNodeFlags_Framed))
				// ImGui::Text("Bool");
				// ImGui::Separator(false);
				{
					auto& param_table = _curr_animator_component->GetParamTable<anim::BoolParam>();
					for (auto& param : param_table)
					{
						const bool change = display_param(param.second, _selected_param_type == anim::AnimDataType::BOOL && _selected_param == param.second.name);
						if (change)
						{
							_selected_param_type = anim::AnimDataType::BOOL;
							_selected_param = param.second.name;
						}
					}

					// Rename outside the loop
					if (!rename_to.empty())
					{
						_curr_animator_component->RenameParam<anim::BoolParam>(rename_param, rename_to);
						rename_param.clear();
						rename_to.clear();
					}
				}
				
				ImGui::SetNextTreeNodeOpen(true, ImGuiCond_Once);
				if (ImGui::CollapsingHeader("Trigger", ImGuiTreeNodeFlags_Framed))
				// ImGui::Text("Trigger");
				// ImGui::Separator(false);
				{
					auto& param_table = _curr_animator_component->GetParamTable<anim::TriggerParam>();
					for (auto& param : param_table)
					{
						const bool change = display_param(param.second, _selected_param_type == anim::AnimDataType::TRIGGER && _selected_param == param.second.name);
						if (change)
						{
							_selected_param_type = anim::AnimDataType::TRIGGER;
							_selected_param = param.second.name;
						}
					}

					// Rename outside the loop
					if (!rename_to.empty())
					{
						_curr_animator_component->RenameParam<anim::TriggerParam>(rename_param, rename_to);
						rename_param.clear();
						rename_to.clear();
					}
				}
				ImGui::Separator(false);
			}
			
			ImGui::EndTabItem();
		}
	}
	void IGE_AnimatorWindow::drawParamsContextMenu()
	{
		if (ImGui::BeginPopup("params_context_menu", ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoScrollWithMouse | ImGuiWindowFlags_NoResize))
		{
			if (ImGui::Selectable("Int"))
				_curr_animator_component->AddParam<anim::IntParam>("New Int");
			if (ImGui::Selectable("Float"))
				_curr_animator_component->AddParam<anim::FloatParam>("New Float");
			if (ImGui::Selectable("Bool"))
				_curr_animator_component->AddParam<anim::BoolParam>("New Bool");
			if (ImGui::Selectable("Trigger"))
				_curr_animator_component->AddParam<anim::TriggerParam>("New Trigger");
			ImGui::EndPopup();
		}
	}
	void IGE_AnimatorWindow::drawStatesTab()
	{
		static char buf[50];
		static bool just_rename = false;
		static int rename_index = -1;
		
		static const ImVec2 selectable_offset{ 0, ImGui::GetStyle().FramePadding.y };
		static const ImVec2 selectable_size{ 0, ImGui::GetFrameHeightWithSpacing() * _state_selectable_num_items + 3.0f + ImGui::GetStyle().FramePadding.y * 4 };

		const auto display_transitions_drop_down = [&](size_t state_index)
		{
			auto& layer = _curr_animator_component->layers[_selected_layer];
			auto& state = layer.GetAnimationState(state_index);
			if (!state.valid)
				return;

			if (state.display_transitions_drop_down)
			{
				const auto cursor_x = ImGui::GetCursorPosX();
				for (size_t transition_index = 1; transition_index < state.transitions.size(); ++transition_index)
				{
					auto& curr_transition = state.GetTransition(transition_index);
					if (!curr_transition.valid)
						continue;

					// Use pointer value as ID
					ImGui::PushID(r_cast<size_t>(&curr_transition));

					// ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_Bullet | ImGuiTreeNodeFlags_FramePadding;
					const bool selected = (state_index == _selected_state) && (_selected_transition == transition_index);
					// if (selected)
					// 	flags |= ImGuiTreeNodeFlags_Selected;

					// Display tree node
					string transition_from = layer.GetAnimationState(curr_transition.transition_from_index).name;
					string transition_to = layer.GetAnimationState(curr_transition.transition_to_index).name;

					ImGui::Indent();

					ImGui::BeginGroup();

					const auto prev_cursor_pos = ImGui::GetCursorPos();
					ImGui::PushStyleColor(ImGuiCol_Header, _selectable_bg_col);
					ImGui::PushStyleColor(ImGuiCol_HeaderActive, _selectable_active_col);
					if (selected)
						ImGui::PushStyleColor(ImGuiCol_HeaderHovered, _selectable_bg_col);
					else
						ImGui::PushStyleColor(ImGuiCol_HeaderHovered, _selectable_hovered_col);
					if (ImGui::Selectable("##transition_drop_down", selected, ImGuiSelectableFlags_AllowItemOverlap | ImGuiSelectableFlags_PressedOnClick, selectable_size))
					{
						selectTransition(_selected_layer, _selected_state, transition_index);
						_show_transition = true;
						_display_mode = AnimatorDisplayMode::State;
					}
	
					if (ImGui::IsItemHovered() && ImGui::IsWindowHovered())
					{
						if(ImGui::IsMouseDoubleClicked(0))
						{
							selectTransition(_selected_layer, _selected_state, _selected_transition);
						}
						else if( ImGui::IsMouseClicked(1))
						{
							ImGui::OpenPopup("states_context_menu");
						}
					}


					ImGui::PopStyleColor(3);
					ImGui::Separator(false);
					const auto new_cursor_pos = ImGui::GetCursorPos();
					ImGui::SetCursorPos(prev_cursor_pos + selectable_offset);
					
					// Display transition name
					ImGui::Text(transition_from.data());
					ImGui::SameLine();
					arrow();
					ImGui::SameLine(0.0f, ImGui::GetFrameHeight() + ImGui::GetStyle().FramePadding.x * 2);
					ImGui::Text(transition_to.data());

					ImGui::ProgressBar(0.0f, ImVec2(-1, 3), nullptr);

					ImGui::EndGroup();
					ImGui::Unindent();
					ImGui::SetCursorPos(new_cursor_pos);
					
					ImGui::PopID();
				}

				ImGui::SetCursorPosX(cursor_x);
			}
		};

		if (ImGui::BeginTabItem("States", nullptr, ImGuiTabItemFlags_NoCloseWithMiddleMouseButton))
		{
			if (_curr_animator_component)
			{
				auto& curr_layer = _curr_animator_component->layers[_selected_layer];

				if (ImGui::Button("Add"))
				{
					curr_layer.AddAnimation(RscHandle<anim::Animation>{});
				}
				ImGui::SameLine();

				const bool can_remove = _selected_state != 0 && curr_layer.anim_states.size() > 1;
				if (!can_remove)
					ImGuidk::PushDisabled();
				if (ImGui::Button("Delete"))
				{
					curr_layer.RemoveAnimation(_selected_state);
					_selected_state = std::max(size_t{ 0 }, _selected_state - 1);
				}
				if (!can_remove)
					ImGuidk::PopDisabled();

				ImGui::Separator(false);

				for (int i = 1; i < curr_layer.anim_states.size(); ++i)
				{
					auto& state = curr_layer.anim_states[i];
					strcpy_s(buf, state.name.data());
					ImGui::PushID(i);

					const auto prev_cursor_pos = ImGui::GetCursorPos();
					const bool selected = i == _selected_state;
					ImGui::PushStyleColor(ImGuiCol_Header, _selectable_bg_col);
					ImGui::PushStyleColor(ImGuiCol_HeaderActive, _selectable_active_col);
					if (selected)
						ImGui::PushStyleColor(ImGuiCol_HeaderHovered, _selectable_bg_col);
					else
						ImGui::PushStyleColor(ImGuiCol_HeaderHovered, _selectable_hovered_col);
					if (ImGui::Selectable("##state_selectable", selected, ImGuiSelectableFlags_AllowItemOverlap, selectable_size))
					{
						selectState(_selected_layer, i);
					}
					ImGui::PopStyleColor(3);
					ImGui::Separator(false);
					const auto new_cursor_pos = ImGui::GetCursorPos();

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
					float align_checkbox = ImGui::GetContentRegionAvailWidth() - ImGui::CalcTextSize("Transitions").x - ImGui::GetFrameHeight() - (ImGui::GetStyle().FramePadding.x * 2);
					align_checkbox = std::max(align_checkbox, 0.0f);
					ImGui::SameLine(align_checkbox);
					ImGui::Text("Transitions");
					ImGui::SameLine();
					ImGui::Checkbox("##transition_drop_down", &state.display_transitions_drop_down);
					

					float prog_bar = 0.0f;
					if (curr_layer.curr_state.index == i)
						prog_bar = curr_layer.curr_state.normalized_time;
					else if (curr_layer.blend_state.index == i)
						prog_bar = curr_layer.blend_state.normalized_time;

					ImGui::ProgressBar(prog_bar, ImVec2(-1, 3), nullptr);
					ImGui::Unindent(ImGui::GetStyle().FramePadding.x);
					ImGui::SetCursorPos(new_cursor_pos);

					display_transitions_drop_down(i);
					ImGui::PopID();
					
				}

				ImGui::NewLine();
				
			}
			ImGui::EndTabItem();
		}
	}
	void IGE_AnimatorWindow::drawStatesContextMenu()
	{
		if (ImGui::BeginPopup("states_context_menu", ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoScrollWithMouse | ImGuiWindowFlags_NoResize))
		{
			auto& curr_layer = _curr_animator_component->layers[_selected_layer];
			auto& curr_state = curr_layer.GetAnimationState(_selected_state);

			if (ImGui::Selectable("Set Entry State"))
			{
				curr_layer.default_index = _selected_state;
				curr_layer.curr_state.Reset();
				curr_layer.ResetBlend();

				curr_layer.curr_state.index = _selected_state;
			}
			const bool is_blend_tree = curr_state.IsBlendTree();

			if (is_blend_tree)
				ImGuidk::PushDisabled();
			if (ImGui::Selectable("Convert to Blend Tree"))
			{

			}
			if (is_blend_tree)
				ImGuidk::PopDisabled();

			if (!is_blend_tree)
				ImGuidk::PushDisabled();
			if (ImGui::Selectable("Convert to Basic State"))
			{

			}
			if (!is_blend_tree)
				ImGuidk::PopDisabled();

			ImGui::EndPopup();
		}
	}
	void IGE_AnimatorWindow::drawBoneMaskTab()
	{
	}

	void IGE_AnimatorWindow::inspectLayer(size_t layer_index)
	{
		UNREFERENCED_PARAMETER(layer_index);
		//auto& curr_layer = _curr_animator_component->layers[layer_index];
	}

	void IGE_AnimatorWindow::inspectState(size_t layer_index, size_t state_index)
	{
		static char buf[50];
		auto& curr_layer = _curr_animator_component->layers[layer_index];
		auto& curr_state = curr_layer.GetAnimationState(state_index);

		if (!curr_state.valid)
			return;

		auto& state_data = *curr_state.GetBasicState();

		ImGui::PushID(_selected_state);
		strcpy_s(buf, curr_state.name.data());

		float width_avail = (ImGui::GetContentRegionAvailWidth() - ImGui::CalcTextSize("Animation State").x);
		float item_width = width_avail * 0.8f;
		float offset = ImGui::CalcTextSize("Animation State").x + width_avail - item_width;

		ImGui::Text("Animation State");

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
		ImGui::Separator(false);

		ImGui::Text("Transitions");
		ImGui::PushItemWidth(-1);
		auto bg_col = ImGui::GetStyle().Colors[ImGuiCol_FrameBg];
		bg_col.w = bg_col.w * 0.4f;
		ImGui::PushStyleColor(ImGuiCol_FrameBg, bg_col);
		const bool list_is_open = ImGui::ListBoxHeader("##transitions", curr_state.transitions.size(), 5);
		ImGui::PopStyleColor();

		if (list_is_open)
		{
			for (size_t i = 1; i < curr_state.transitions.size(); ++i)
			{
				ImGui::PushID(i);
				auto& transition = curr_state.GetTransition(i);
				if (!transition.valid)
					break;
				bool selected = _selected_transition == i;
				string transition_from = curr_layer.GetAnimationState(transition.transition_from_index).name;
				string transition_to = curr_layer.GetAnimationState(transition.transition_to_index).name;
				string transition_title = transition_from + "\t->\t" + transition_to;

				const auto curr_pos = ImGui::GetCursorPos() + ImGui::GetStyle().FramePadding;
				if(ImGui::Selectable("##transition_display", selected, ImGuiSelectableFlags_AllowItemOverlap))
				{
					selectTransition(_selected_layer, _selected_state, i);
					_show_transition = true;
					_display_mode = State;
				}

				if (ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(0))
				{
					selectTransition(_selected_layer, _selected_state, i);
				}

				const auto next_pos = ImGui::GetCursorPos();
				ImGui::SetCursorPos(curr_pos);

				// Render text
				ImGui::BeginGroup();
				{
					ImGui::Text(transition_from.data());

					const float align_arrow = ImGui::GetContentRegionAvailWidth() * 0.5f - ImGui::GetFontSize() * 0.5f - ImGui::GetStyle().FramePadding.x;
					const float align_text = ImGui::GetContentRegionAvailWidth() - ImGui::CalcTextSize(transition_to.data()).x - ImGui::GetStyle().FramePadding.x;

					ImGui::SameLine(align_arrow);
					arrow();

					ImGui::SameLine(align_text);
					ImGui::Text(transition_to.data());
				}
				ImGui::EndGroup();

				ImGui::SetCursorPos(next_pos);

				
				ImGui::PopID();
			}

			ImGui::ListBoxFooter();
		}
		ImGui::PopItemWidth();

		if (ImGui::Button("Add"))
		{
			curr_state.AddTransition(_selected_state, 0);
		}
		ImGui::SameLine();

		const bool can_remove = _selected_transition > 0 || _selected_transition < curr_state.transitions.size();
		if (!can_remove)
			ImGuidk::PushDisabled();

		if (ImGui::Button("Delete"))
		{
			curr_state.RemoveTransition(_selected_transition);
			const bool showing_transition = _show_transition;
			selectTransition(_selected_layer, _selected_state, std::max(size_t{ 0 }, _selected_transition - 1));
			if (_selected_transition > 0)
				_show_transition = showing_transition;
		}

		if (!can_remove)
			ImGuidk::PopDisabled();

		if (!has_valid_clip)
			ImGuidk::PopDisabled();

		ImGui::NewLine();
		ImGui::Separator(false);
		ImGui::PopID();

		if (_show_transition)
		{
			inspectTransition(layer_index, state_index, _selected_transition);
		}
	}

	void IGE_AnimatorWindow::inspectTransition(size_t layer_index, size_t state_index, size_t transition_index)
	{
		static char buf[50];
		auto& curr_layer = _curr_animator_component->layers[layer_index];
		auto& curr_state = curr_layer.GetAnimationState(state_index);
		auto& curr_transition = curr_state.GetTransition(transition_index);
		auto& transition_from_state = curr_layer.GetAnimationState(curr_transition.transition_from_index);
		auto& transition_to_state = curr_layer.GetAnimationState(curr_transition.transition_to_index);

		if (!curr_transition.valid)
			return;

		if (!curr_transition.valid)
			return;

		ImGui::PushID(&curr_transition);

		if (ImGui::Selectable(transition_from_state.name.data(), false, ImGuiSelectableFlags_None, ImGui::CalcTextSize(transition_from_state.name.data())))
		{
			selectState(_selected_layer, curr_transition.transition_from_index);
		}

		ImGui::SameLine();
		arrow();
		ImGui::SameLine(0.0f, ImGui::GetFrameHeight() + ImGui::GetStyle().FramePadding.x * 3);
		if (transition_to_state.valid)
		{
			if (ImGui::Selectable(transition_to_state.name.data(), false, ImGuiSelectableFlags_None, ImGui::CalcTextSize(transition_to_state.name.data())))
			{
				selectState(_selected_layer, curr_transition.transition_to_index);
			}
		}
		else
			ImGui::Text(transition_to_state.name.data());
		ImGui::Separator(false);

		const float width_avail = (ImGui::GetContentRegionAvailWidth() - ImGui::CalcTextSize("Transition Duration").x);
		const float item_width = width_avail * 0.8f;
		const float offset = ImGui::GetContentRegionAvailWidth() - item_width;

		ImGui::Text("Transition To: ");
		ImGui::SameLine(offset);
		ImGui::PushItemWidth(item_width);
		if (ImGui::BeginCombo("##transition_to", transition_to_state.name.data()))
		{
			for (size_t i = 0; i < curr_layer.anim_states.size(); ++i)
			{
				if (i == transition_index)
					continue;

				auto& s = curr_layer.anim_states[i];
				bool selected = i == curr_transition.transition_to_index;
				if (ImGui::Selectable(s.name.data(), selected))
				{
					curr_transition.transition_to_index = i;
				}
			}

			ImGui::EndCombo();
		}
		ImGui::PopItemWidth();

		if (!transition_to_state.valid)
			ImGuidk::PushDisabled();
		ImGui::Text("Has Exit Time");
		ImGui::SameLine(offset);
		ImGui::Checkbox("##has_exit_time", &curr_transition.has_exit_time);

		// Exit time must be positive
		ImGui::Text("Exit Time");
		ImGui::SameLine(offset);
		ImGui::PushItemWidth(item_width);
		if (ImGui::InputFloat("##exit_time", &curr_transition.exit_time))
			curr_transition.exit_time = max(curr_transition.exit_time, 0.0f);
		ImGui::PopItemWidth();
		ImGui::NewLine();

		ImGui::Text("Transition Duration");
		ImGui::SameLine(offset);
		ImGui::PushItemWidth(item_width);
		if (ImGui::InputFloat("##transition_dur", &curr_transition.transition_duration))
			curr_transition.transition_duration = max(curr_transition.transition_duration, 0.0f);
		ImGui::PopItemWidth();

		ImGui::Text("Transition Offset");
		ImGui::SameLine(offset);
		ImGui::PushItemWidth(item_width);
		if (ImGui::InputFloat("##transition_offset", &curr_transition.transition_offset))
			curr_transition.transition_offset = max(curr_transition.transition_offset, 0.0f);
		ImGui::PopItemWidth();

		if (!transition_to_state.valid)
			ImGuidk::PopDisabled();

		ImGui::NewLine();
		ImGui::Separator(false);
		ImGui::PopID();
	}

	void IGE_AnimatorWindow::displayConditions(size_t layer_index, size_t state_index, size_t transition_index)
	{
		auto& curr_layer = _curr_animator_component->layers[layer_index];
		auto& curr_state = curr_layer.GetAnimationState(state_index);
		auto& curr_transition = curr_state.GetTransition(transition_index);
		
		ImGui::Text("Conditions");
		ImGui::PushItemWidth(-1);
		auto bg_col = ImGui::GetStyle().Colors[ImGuiCol_FrameBg];
		bg_col.w = bg_col.w * 0.4f;
		ImGui::PushStyleColor(ImGuiCol_FrameBg, bg_col);
		const bool list_is_open = ImGui::ListBoxHeader("##conditions", curr_transition.conditions.size(), 5);
		ImGui::PopStyleColor();

		if (list_is_open)
		{
			for (size_t i = 0; i < curr_transition.conditions.size(); ++i)
			{
				ImGui::PushID(i);
				auto& condition = curr_transition.conditions[i];
				
				const bool selected = _selected_condition == i;
				
				const auto curr_pos = ImGui::GetCursorPos() + ImGui::GetStyle().FramePadding;
				if (ImGui::Selectable("##conditions_display", selected, ImGuiSelectableFlags_AllowItemOverlap))
				{
					_selected_condition = i;
				}

				const auto next_pos = ImGui::GetCursorPos();
				ImGui::SetCursorPos(curr_pos);

				// Render text
				ImGui::BeginGroup();
				{
					
					//ImGui::BeginCombo()
				}
				ImGui::EndGroup();

				ImGui::SetCursorPos(next_pos);


				ImGui::PopID();
			}

			ImGui::ListBoxFooter();
		}
		ImGui::PopItemWidth();
	}

	void IGE_AnimatorWindow::animatorInspector()
	{
		ImGui::PushStyleColor(ImGuiCol_ChildBg, ImGui::GetStyleColorVec4(ImGuiCol_WindowBg));
		ImGui::PushStyleVar(ImGuiStyleVar_ChildRounding, 2.0f);

		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 4.0f));
		ImGui::BeginChild("RightChild", ImVec2(), false, ImGuiWindowFlags_AlwaysUseWindowPadding);
		ImGui::PopStyleVar(2);
		ImGui::PopStyleColor();

		if (ImGui::BeginTabBar("Animator Inspector##TB", ImGuiTabBarFlags_NoCloseWithMiddleMouseButton))
		{
			if (ImGui::BeginTabItem("Animator Inspector", nullptr, ImGuiTabItemFlags_NoCloseWithMiddleMouseButton))
			{
				if (_curr_animator_component)
				{
					switch (_display_mode)
					{
					case AnimatorDisplayMode::Layer:
					{
						inspectLayer(_selected_layer);
						break;
					}
					case AnimatorDisplayMode::State:
					{
						inspectState(_selected_layer, _selected_state);
						break;
					}
					case AnimatorDisplayMode::Transition:
					{
						inspectTransition(_selected_layer, _selected_state, _selected_transition);
						break;
					}

					case AnimatorDisplayMode::None:
						break;
					}
				}
				ImGui::EndTabItem();
			}
			ImGui::EndTabBar();
		}
		ImGui::EndChild();
	}

	void IGE_AnimatorWindow::resetSelection()
	{
		_display_mode = None;
		_selected_layer = 0;
		_selected_state = 0;
		_selected_transition = 0;
		_selected_condition = -1;
		_show_transition = false;
	}

	void IGE_AnimatorWindow::selectLayer(size_t layer_index)
	{
		resetSelection();
		_selected_layer = layer_index;
		_display_mode = Layer;
	}

	void IGE_AnimatorWindow::selectState(size_t layer_index, size_t state_index)
	{
		resetSelection();
		_selected_layer = layer_index;
		_selected_state = state_index;
		_display_mode = State;
	}

	void IGE_AnimatorWindow::selectTransition(size_t layer_index, size_t state_index, size_t transition_index)
	{
		resetSelection();
		_selected_layer = layer_index;
		_selected_state = state_index;
		_selected_transition = transition_index;
		_display_mode = Transition;
	}
}