//////////////////////////////////////////////////////////////////////////////////
//@file		IGE_MainWindow.h
//@author	Muhammad Izha B Rahim
//@param	Email : izha95\@hotmail.com
//@date		30 AUG 2019
//@brief	

/*
This window displays the editor window where you can select and modify gameobjects.
*/
//////////////////////////////////////////////////////////////////////////////////



#pragma once
#include <editor/windows/IGE_IWindow.h>

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

		bool is_controlling_WASDcam = false;
		bool is_controlling_Pancam = false;

		const float yaw_rotation_multiplier		= 0.1f; //When you hold right click and move mouse sideways
		const float pitch_rotation_multiplier	= 0.05f; //When you hold right click and move mouse up/downwards
		const float cam_vel						= 1.f;


		float pan_multiplier					= 0.1f;
		const float pan_multiplier_on_scroll	= 1.1f; //This is additive and clamped. When zooming out pan_multiplier*pan_multiplier_on_scroll, when zooming in pan_multiplier/pan_multiplier_on_scroll
		const float default_pan_multiplier		= 0.1f; //When on focus, this resets the pan_multiplier


		void UpdateWASDMouseControl();
		void UpdatePanMouseControl();


	};





}