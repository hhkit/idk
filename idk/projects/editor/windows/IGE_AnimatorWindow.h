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

		// The current animator component being displayed
		Handle<PrefabInstance> _prefab_inst;
		Handle<Animator> _curr_animator_component;
		AnimatorDisplayMode _display_mode;
		size_t _selected_layer = 0;
		size_t _selected_state = 0;
		size_t _selected_transition = 0;
		int _selected_condition = -1;
		size_t _selected_bone_mask_index = 0;

		anim::AnimDataType _selected_param_type = anim::AnimDataType::NONE;
		string _selected_param;

		bool _show_transition = false;

		bool _layers_changed = false;
		bool _params_changed = false;
		
		// Colors for all the widgets
		ImVec4 _selectable_bg_col;
		ImVec4 _selectable_active_col;
		ImVec4 _selectable_hovered_col;

		ImVec4 _blend_prog_col;
		// ImVec2 _layer_selectable_display_size;
		size_t _layer_selectable_num_items;
		size_t _state_selectable_num_items;

		void layersAndParams();
		void statesAndBoneMask();
		void animatorInspector();

		void drawLayersTab();
		bool drawLayersContextMenu();
		void drawParamsTab();
		bool drawParamsContextMenu();
		void drawStatesTab();
		bool drawStatesContextMenu();
		void drawBoneMaskTab();
		bool bone_mask_recurse(const vector<Handle<GameObject>>& child_objects, vector<uint8_t>& bone_mask, size_t index, bool highlight);
		void bone_mask_set_children(const vector<Handle<GameObject>>& child_objects, vector<uint8_t>& bone_mask, size_t index, uint8_t val);

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

		void arrow();
	};
}