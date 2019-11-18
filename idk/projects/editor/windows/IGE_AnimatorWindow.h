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
		int _selected_transition = -1;
		bool _show_transition = false;
		
		// Colors for all the widgets
		ImVec4 _selectable_bg_col;
		ImVec4 _selectable_active_col;
		ImVec4 _selectable_hovered_col;

		ImVec4 _blend_prog_col;
		ImVec2 _layer_selectable_display_size;
		ImVec2 _state_selectable_display_size;
		ImVec2 _selectable_offset;

		float align_right(float width_avail, float size);
		void separator();

		void drawLeftCol();
		
		void drawCanvas();
		
		void drawBlendTreeCanvas();
		void drawAnimatorInspector();

		void drawLayersTab();
		void drawParamsTab();
		void drawStatesTab();

		bool canvasContextMenu();
		bool drawConnection(ImVec2 p1, ImVec2 p2);

		void resetSelection();
		
	};
}