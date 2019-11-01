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

namespace idk {
	class IGE_SceneView :
		public IGE_IWindow
	{
	public:
		IGE_SceneView();

		virtual void BeginWindow() override;
		virtual void Update() override;

		void SetTexture(void* textureToRender); //Place the camera texture here. EG: SetTexture((void*)(intptr_t)myGluintTexture);

		vec2 GetScreenSize(); //To get the size of the frame being drawn onto the window.

		vec2 GetMousePosInWindow();
		vec2 GetMousePosInWindowNormalized();
	protected:


	private:

		ImTextureID sceneTexture = nullptr;
        vec2 draw_rect_offset;
        vec2 draw_rect_size;

		bool is_controlling_WASDcam = false;
		bool is_controlling_Pancam = false;

		const float yaw_rotation_multiplier		= 0.1f; //When you hold right click and move mouse sideways
		const float pitch_rotation_multiplier	= 0.05f; //When you hold right click and move mouse up/downwards
		float cam_vel							= 1.f;
		const float cam_vel_additive			= 0.1f;
		const float min_cam_vel					= 0.1f;
		const float cam_vel_shift_multiplier	= 4.f;


		const float pan_multiplier				= 0.01f;

		bool	display_grid						= false;
		//void	DrawGridControl();

		float	translate_snap_val[3]				= {0.0f,0.0f,0.0f};
		float	rotate_snap_val						= 0;
		float	scale_snap_val						= 0;
		const vector<float> translate_snap_type		= { 0,0.5,1,5,10,50,100,500 };
		const vector<float> rotate_snap_type		= { 0,1,5,15,30,45,90	};
		const vector<float> scale_snap_type			= { 0,0.5f,0.25f,0.125f,0.0625f };

		void DrawSnapControl();


		std::pair<Handle<GameObject>, phys::raycast_result> GetClosestGameObjectFromCamera(vector<Handle<GameObject>>& refVector, vector<phys::raycast_result>& rayResult);
		//Debug ray
		ray currRay;


		void UpdateWASDMouseControl();
		void UpdatePanMouseControl();
		void UpdateScrollMouseControl();

		void UpdateGizmoControl();

		void MoveMouseToWindow(); //Moves the mouse to the middle of the sceneView

		void ImGuizmoManipulateUpdate(Handle<Transform>& originalTransform); //Call after ImGuizmo::Manipulate


		float gizmo_matrix[16]{};

		mat4 GenerateMat4FromGizmoMatrix();

		bool is_being_modified = false;

		ray GenerateRayFromCurrentScreen();

		POINT prevMouseScreenPos{};	 //Using windows getcursor instead of imgui cos its being a meanie
		POINT currMouseScreenPos{};	 //Using windows getcursor instead of imgui cos its being a meanie

	};





}