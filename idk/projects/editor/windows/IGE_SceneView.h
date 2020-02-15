//////////////////////////////////////////////////////////////////////////////////
//@file		IGE_MainWindow.h
//@author	Muhammad Izha B Rahim
//@param	Email : izha95\@hotmail.com
//@date		21 OCT 2019
//@brief	

/*
This window displays the editor window where you can select and modify gameobjects.
*/
//////////////////////////////////////////////////////////////////////////////////
#pragma once

#include <editor/windows/IGE_IWindow.h>
#include <imgui/imgui.h>
#include <phys/PhysicsSystem.h>
#include <gfx/ColorPickResult.h>

namespace idk 
{
	struct PickState
	{
		bool is_multi_select = false;
	};

	class IGE_SceneView :
		public IGE_IWindow
	{
	public:
		IGE_SceneView();

		virtual void BeginWindow() override;
		virtual void Update() override;

		vec2 GetScreenSize(); //To get the size of the frame being drawn onto the window.
		vec2 GetMousePosInWindow();
		vec2 GetMousePosInWindowNormalized();

		void SetOrbitPoint(vec3 pt);

	private:
        vec2 draw_rect_offset;
        vec2 draw_rect_size;

		vec3 orbit_point;
		float distance_to_orbit_point;	//When WASD control is activated, this is used to move the orbit_point! Only used for WASD control

		bool is_controlling_WASDcam   = false;
		bool is_controlling_Pancam	  = false;
		bool is_controlling_ALTscroll = false;
		bool is_controlling_ALTorbit  = false;

		float cam_vel = 1.f;

		static constexpr float yaw_rotation_multiplier		= 0.1f; //When you hold right click and move mouse sideways
		static constexpr float pitch_rotation_multiplier	= 0.05f; //When you hold right click and move mouse up/downwards
		static constexpr float cam_vel_additive				= 0.5f;
		static constexpr float min_cam_vel					= 0.1f;
		static constexpr float cam_vel_shift_multiplier		= 4.f;
		static constexpr float orbit_strength				= 0.5f;
		static constexpr float pan_multiplier				= 0.1f;

		//bool	display_grid						= false;
		//void	DrawGridControl();

		float	translate_snap_val[3]				= {0.0f,0.0f,0.0f};
		float	rotate_snap_val						= 0;
		float	scale_snap_val						= 0;
		const vector<float> translate_snap_type		= { 0,0.5,1,5,10,50,100,500 };
		const vector<float> rotate_snap_type		= { 0,1,5,15,30,45,90	};
		const vector<float> scale_snap_type			= { 0,0.5f,0.25f,0.125f,0.0625f };

		std::optional<std::pair<ColorPickResult,PickState>> last_pick;

		float gizmo_matrix[16]{};
		vector<mat4> original_matrices;

		bool is_being_modified = false;
		ivec2 cachedMouseScreenPos{};
		ivec2 prevMouseScreenPos{};	 //Using windows getcursor instead of imgui cos its being a meanie
		ivec2 currMouseScreenPos{};	 //Using windows getcursor instead of imgui cos its being a meanie

		void DrawSnapControl();
		void DrawGlobalAxes();

		void UpdateWASDMouseControl();
		void UpdatePanMouseControl(); //MiddleMouse
		void UpdateScrollMouseControl(); //Scroll
		void UpdateScrollMouseControl2(); //Alt + RightMouse
		void UpdateOrbitControl(); //Alt + LeftMouse
		
		void UpdateGizmoControl();

		void CenterMouseInWindow();

		void ImGuizmoManipulateUpdate(Handle<Transform>& originalTransform); //Call after ImGuizmo::Manipulate
		mat4 GenerateMat4FromGizmoMatrix();

		ray WindowsPointToRay(vec2 vp_pos);
		ray GenerateRayFromCurrentScreen();
		
	};





}