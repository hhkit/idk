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

	};





}