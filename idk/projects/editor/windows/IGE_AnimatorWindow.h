#pragma once

#include <editor/windows/IGE_IWindow.h>
#include <imgui/imnodes/ImNodes.h>

namespace idk {
	class IGE_AnimatorWindow :
		public IGE_IWindow
	{
	public:
		IGE_AnimatorWindow();

		virtual void BeginWindow() override;
		virtual void Initialize() override;
		virtual void Update() override;
		~IGE_AnimatorWindow();
	private:
		enum AnimatorDisplayMode
		{
			None = 0,
			Layer,
			Transition,
			State,
			BlendTree,
			MAX
		};

		ImNodes::CanvasState _canvas;
		// The current animator component being displayed
		Handle<Animator> _curr_animator_component;
		AnimatorDisplayMode _display_mode;
		size_t _selected_layer = 0;
		size_t _selected_state = 0;
		size_t _selected_transition = 0;

		int _selected_condition = -1;

		anim::AnimDataType _selected_param_type = anim::AnimDataType::NONE;
		string _selected_param;

		bool _show_transition = false;
		
		// Colors for all the widgets
		ImVec4 _selectable_bg_col;
		ImVec4 _selectable_active_col;
		ImVec4 _selectable_hovered_col;

		ImVec4 _blend_prog_col;
		// ImVec2 _layer_selectable_display_size;
		size_t _layer_selectable_num_items;
		size_t _state_selectable_num_items;

		void arrow();

		void layersAndParams();
		void statesAndBoneMask();
		void animatorInspector();

		void drawLayersTab();
		void drawLayersContextMenu();
		void drawParamsTab();
		void drawParamsContextMenu();
		void drawStatesTab();
		void drawStatesContextMenu();
		void drawBoneMaskTab();

		void inspectLayer(size_t layer_index);
		void inspectState(size_t layer_index, size_t state_index);
		void inspectBasicState(size_t layer_index, size_t state_index);
		void inspectBlendTree(size_t layer_index, size_t state_index);
		void inspectTransition(size_t layer_index, size_t state_index, size_t transition_index);
		void displayConditions(size_t layer_index, size_t state_index, size_t transition_index);

		void resetSelection();

		void selectLayer(size_t layer_index);
		void selectState(size_t layer_index, size_t state_index);
		void selectTransition(size_t layer_index, size_t state_index, size_t transition_index);
	};
}