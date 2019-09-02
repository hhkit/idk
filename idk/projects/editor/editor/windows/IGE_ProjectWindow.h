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
	class IGE_ProjectWindow :
		public IGE_IWindow
	{
	public:
		IGE_ProjectWindow();

		virtual void BeginWindow() override;
		virtual void Update() override;

	
	protected:


	private:


	};





}